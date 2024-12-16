#include    "graphics.h"
#include    <iostream>
#include    <stack>
#include    <cmath>
#include    <string>
#include    <vector>
#include    <map>
#include    <Windows.h>
using namespace std;


//butoane

struct buton {
    int x1, y1, btnWIDTH, btnHEIGHT;
    string nume;
    bool isHover;

    buton(int x1, int y1, int btnWIDTH, int btnHEIGHT, string nume) : x1(x1), y1(y1), btnWIDTH(btnWIDTH), btnHEIGHT(btnHEIGHT), nume(nume), isHover(false) {}

    void Deseneaza() {
        setfillstyle(SOLID_FILL, isHover ? DARKGRAY : LIGHTGRAY);
        bar(x1, y1, x1 + btnWIDTH, y1 + btnHEIGHT);
        setbkcolor(isHover ? DARKGRAY : LIGHTGRAY);
        setcolor(BLACK);
        outtextxy((x1 + (x1 + btnWIDTH)) / 2 - 20, (y1 + (y1 + btnHEIGHT)) / 2 - 10, const_cast<char*>(nume.c_str()));
    }

    bool isClicked(int mx, int my) {
        return mx >= x1 && mx <= (x1 + btnWIDTH) && my >= y1 && my <= (y1 + btnHEIGHT);
    }
};
struct TextBox {
    int x1, y1, x2, y2; // Coordonatele dreptunghiului
    string text;        // Textul curent
    bool active;        // Este activ (selectat de utilizator)?

    // Desenarea TextBox-ului
    void draw() {
        // Culoarea dreptunghiului depinde de starea activă
        setcolor(active ? LIGHTCYAN : BLACK);
        rectangle(x1, y1, x2, y2); // Dreptunghiul
        setbkcolor(WHITE);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // Stil text
        outtextxy(x1 + 5, y1 + 5, const_cast<char*>(text.c_str())); // Text în interior
    }

    // Detectează clicuri pe caseta de text
    bool isClicked(int mx, int my) {
        return mx >= x1 && mx <= x2 && my >= y1 && my <= y2;
    }

    // Adaugă caractere la text
    void addChar(char ch) {
        if (ch == '\b') { // Backspace
            if (!text.empty()) text.pop_back();
        }
        else {
            text.push_back(ch);
        }
    }
};
vector<buton> Butoane;

TextBox caseta1={30,70,630,130,"",false}, caseta2, caseta3;

// Dimensiunile ecranului grafic
int WIDTH = GetSystemMetrics(SM_CXSCREEN);
int HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int ORIGIN_X = WIDTH / 2;
int ORIGIN_Y = HEIGHT / 2;
int MENU_HEIGHT = 60;
int CAMERA_X = 0;
int CAMERA_Y = 0;
bool FULSCREEN = 1;
bool isDarkModeOn = 0;

int SCALE = 50;



void intToChar(int number, char* buffer, size_t bufferSize) {
    bool isNegative = (number < 0);
    if (isNegative) number = -number;

    int index = 0;
    do {
        if (index >= bufferSize - 1) {
            throw std::overflow_error("Buffer prea mic pentru număr!");
        }
        buffer[index++] = '0' + (number % 10); // Extrage ultima cifră
        number /= 10;                         // Elimină ultima cifră
    } while (number > 0);

    if (isNegative) {
        if (index >= bufferSize - 1) {
            throw std::overflow_error("Buffer prea mic pentru semn negativ!");
        }
        buffer[index++] = '-';
    }

    buffer[index] = '\0'; // Terminator null

    // Inversăm array-ul pentru a obține ordinea corectă
    std::reverse(buffer, buffer + index);
}
//computarea de functii

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^";
}
bool isFunction(const string& token) {
    return token == "sin" || token == "cos" || token == "ln" || token == "sqrt";
}
bool isDigit(char c) {
    return (c - '0' < 10) && (c - '0' >= 0);
}
bool isAlpha(char c) {
    return (c - 'a' >= 0) && (c - 'a' <= 'z' - 'a');
}
int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}
double prelucreazaFunctie(string token, double x) {
    if (token == "sin")
        return sin(x);
    else if (token == "cos")
        return cos(x);
    else if (token == "ln")
        return log(x);
    else if (token == "sqrt")
        return sqrt(x);
    else {
        cout << "Functie invalida\n";
        return 0;
    }
}
vector<string> infixToPostfix(const string& expression) {
    stack<string> operators;         // Stivă pentru operatori și funcții
    vector<string> output;           // Rezultatul în notare postfixată

    int i = 0;
    while (i < expression.size()) {
        char c = expression[i];

        if (c == ' ') { // Ignoră spațiile
            i++;
            continue;
        }

        if (isDigit(c) || c == '.') { // Este număr
            string number;
            while (i < expression.size() && (isDigit(expression[i]) || expression[i] == '.')) {
                number += expression[i];
                i++;
            }
            output.push_back(number);
        }
        else if (isAlpha(c)) { // Este funcție sau variabilă
            string token;
            while (i < expression.size() && isAlpha(expression[i])) {
                token += expression[i];
                i++;
            }
            if (isFunction(token)) {
                operators.push(token);
            }
            else { // Variabilă, cum ar fi `x`
                output.push_back(token);
            }
        }
        else if (c == '(') { // Paranteză deschisă
            operators.push(string(1, c));
            i++;
        }
        else if (c == ')') { // Paranteză închisă
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.pop(); // Scoate paranteza deschisă

            if (!operators.empty() && isFunction(operators.top())) { // Adaugă funcția dacă există
                output.push_back(operators.top());
                operators.pop();
            }
            i++;

        }
        else if (isOperator(string(1, c))) { // Este operator
            string op(1, c);
            while (!operators.empty() &&
                isOperator(operators.top()) &&
                ((op == "^" && precedence(op) < precedence(operators.top())) ||
                    (!(op == "^") && precedence(op) <= precedence(operators.top())))) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(op);
            i++;
        }
        else {
            i++; // Avansează pentru orice alt caracter necunoscut
        }
    }

    // Scoate toți operatorii rămași în stivă
    while (!operators.empty()) {
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

double evaluareExpresie(const vector<string>& postfix, double xVal) {
    stack<double> expresieConvertita;
    for (const string& token : postfix) {
        if (isDigit(token[0]))
            expresieConvertita.push(stod(token));
        else if (token == "x")
            expresieConvertita.push(xVal);
        else if (isFunction(token)) {
            double val = expresieConvertita.top();
            expresieConvertita.pop();
            expresieConvertita.push(prelucreazaFunctie(token, val));
        }
        else if (isOperator(token)) {
            double dr = expresieConvertita.top();expresieConvertita.pop();
            double st = expresieConvertita.top();expresieConvertita.pop();
            if (token == "+") expresieConvertita.push(st + dr);
            if (token == "-") expresieConvertita.push(st - dr);
            if (token == "*") expresieConvertita.push(st * dr);
            if (token == "/") expresieConvertita.push(st / dr);
            if (token == "^") expresieConvertita.push(pow(st, dr));
        }
    }
    return expresieConvertita.top();
}

pair<double, double> ConversieCoord(int screenX, int screenY) {
    double cartesianX = (screenX - ORIGIN_X - CAMERA_X) / static_cast<double>(SCALE);
    double cartesianY = -(screenY - ORIGIN_Y - CAMERA_Y) / static_cast<double>(SCALE);
    return { cartesianX, cartesianY };
}

void Zoom(bool zoomIn)
{
    int scaleVechi = SCALE;
    if (zoomIn) {
        if (SCALE >= 200 && SCALE <= 500) {
            SCALE += 25;
            delay(10);
        }
        else {
            SCALE += 10;
            delay(10);
        }
    }
    else {
        if (SCALE > 25) {
            if (SCALE >= 200) {
                SCALE -= 25;
            }
            else {
                SCALE -= 10;
            }
        }
    }

    double cartesianX = -CAMERA_X / static_cast<double>(scaleVechi);
    double cartesianY = CAMERA_Y / static_cast<double>(scaleVechi);

    CAMERA_X = -cartesianX * SCALE;
    CAMERA_Y = cartesianY * SCALE;

}

void drawAxis(int culoare) {
    //axele de coordonate
    setfillstyle(SOLID_FILL, culoare);
    bar(ORIGIN_X + CAMERA_X, 0, ORIGIN_X + CAMERA_X + 1, HEIGHT); //OY
    bar(0, ORIGIN_Y + CAMERA_Y, WIDTH, ORIGIN_Y + CAMERA_Y + 1); //OX

    int minX = (0 - ORIGIN_X - CAMERA_X) / SCALE;
    int maxX = (WIDTH - ORIGIN_X - CAMERA_X) / SCALE;
    int minY = (ORIGIN_Y + CAMERA_Y - HEIGHT) / SCALE;
    int maxY = (ORIGIN_Y + CAMERA_Y) / SCALE;

    for (int i = minX; i <= maxX;i++) {
        int x = ORIGIN_X + CAMERA_X + i * SCALE;
        int y = ORIGIN_Y + CAMERA_Y;
        bar(x, ORIGIN_Y + CAMERA_Y - 5, x + 1, ORIGIN_Y + CAMERA_Y + 5);
        if (i != 0) {
            char numar[10];
            intToChar(i, numar, 10);
            outtextxy(x-4*((int)log10(i) + 1), y+6, numar);
        }
    }
    for (int i = minY;i <= maxY; i++) {
        int y = ORIGIN_Y + CAMERA_Y - i * SCALE;
        int x = ORIGIN_X + CAMERA_X;
        bar(ORIGIN_X + CAMERA_X - 5, y, ORIGIN_X + CAMERA_X + 5, y + 1);
        if (i != 0) {
            char numar[10];
            intToChar(i, numar, 10);
            outtextxy(x-13*((int)log10(i)+1), y - 7, numar); 
        }
    }

}
void initMeniu() {
    int btnHEIGHT = 20;
    int startX = 10, gap = 1;

    Butoane.push_back(buton(startX, 5, 40, btnHEIGHT, "+"));  // Zoom In
    startX += 40 + gap;
    Butoane.push_back(buton(startX, 5, 40, btnHEIGHT, "-"));  // Zoom Out
    startX += 40 + gap;
    Butoane.push_back(buton(startX, 5, 80, btnHEIGHT, "Domeniu")); // Modifica domeniul
    startX += 80 + gap;
    Butoane.push_back(buton(startX, 5, 80, btnHEIGHT, "Functie")); // Introdu functii
    startX += 80 + gap;
    Butoane.push_back(buton(startX, 5, 80, btnHEIGHT, "Acolada")); // Functie cu acolada
}

void deseneazaMeinu() {
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(0, 0, WIDTH, MENU_HEIGHT);
    for (int i = 0; i < Butoane.size(); i++) {
        Butoane[i].Deseneaza();
    }
}

void handleMouse() {
    int mx = mousex();
    int my = mousey();

    for (auto& btn : Butoane) {
        btn.isHover = btn.isClicked(mx, my); // Actualizează hover

        if (btn.isHover && ismouseclick(WM_LBUTTONDOWN)) {
            clearmouseclick(WM_LBUTTONDOWN);

            if (btn.nume == "+") {
                Zoom(1);
            }
            else if (btn.nume == "-") {
                Zoom(0);
            }
            else if (btn.nume == "Domeniu") {
                double xMin, xMax;
                cout << "Introdu domeniul (xMin xMax): ";
                cin >> xMin >> xMax;
                cout << "Domeniu setat [" << xMin << ", " << xMax << "]" << endl;
            }
            else if (btn.nume == "Functie") {
                caseta1.draw();
            }
            else if (btn.nume == "Acolada") {
                cout << "Functie cu acolada selectata." << endl;
            }
        }
    }
    setbkcolor(15);
}

void drawfunction(vector<string> postfix) //adaugare parametru de culoare pt a desena mai multe functii simultan
{
    double PreviousY = evaluareExpresie(postfix, (0 - ORIGIN_X - CAMERA_X) / SCALE);
    int prev_y = (int)(PreviousY * SCALE);
    prev_y = -prev_y + HEIGHT / 2 + CAMERA_Y;
    putpixel(0, prev_y + HEIGHT / 2 + CAMERA_Y, RED);

    for (double X_POZ = 1;X_POZ < WIDTH;X_POZ += 1) {
        double Y_POZ = evaluareExpresie(postfix, (X_POZ - ORIGIN_X - CAMERA_X) / SCALE); // modific cu coord lui x

        int x = (int)(X_POZ), y = (int)(Y_POZ * SCALE);
        y = -y + HEIGHT / 2 + CAMERA_Y; //conversia pt a plasa punctul corect pe ecran, deoarece originea ecranului este in coltul stanga-sus

        //fix pentru afisarea graficelor punctat

        if (abs(y - prev_y) > 2 && abs(y - prev_y) < 10000) {
            setfillstyle(SOLID_FILL, RED);
            bar(x - 1, y, x, prev_y);
        }
        else
            putpixel(x, y, RED);
        prev_y = y;
        //cout << x << " " << y << '\n';
    }
}


void fereastra()
{
    WIDTH = GetSystemMetrics(SM_CXSCREEN);
    HEIGHT = GetSystemMetrics(SM_CYSCREEN);
    //closegraph();
    ORIGIN_Y = HEIGHT / 2;
    ORIGIN_X = WIDTH / 2;
    initwindow(WIDTH, HEIGHT, "", -3, -3);
    delay(100);
}

void DarkMode(int& fundal, int& axe)
{
    if (isDarkModeOn) {
        fundal = 0;
        axe = 15;
    }
    else {
        fundal = 15;
        axe = 0;
    }
}

bool scroll(int up_or_down)
{
    GET_WHEEL_DELTA_WPARAM(up_or_down);
    return up_or_down;
}

/*bool EroriPostFixata(vector<string> postfix)
{
    for (auto i : postfix) {
        //verificare daca fiecare token din postfixata are o semnificatie
    }
}*/


int main() {
    int aP = 0;
    //int gd = DETECT, gm=DETECT;
    string expression;
    cout << "Introdu expresia matematica: ";
    getline(cin, expression);
    //cout << expression;
    vector<string> postfix = infixToPostfix(expression);
    /*
        //verificare expresie postfixata
    for (auto i : postfix)
        cout << i << ' ';
    */

    initMeniu();

    int culoareFundal, culoareAxe;
    DarkMode(culoareFundal, culoareAxe);
    fereastra();
    drawAxis(culoareAxe);
    deseneazaMeinu();
    //fereastra();
    do {
        //trecerea ferestrei din windowed in fulscreen
        setactivepage(aP);
        setvisualpage(1 - aP);
        cleardevice();



        DarkMode(culoareFundal, culoareAxe);
        setbkcolor(culoareFundal);
        drawAxis(culoareAxe);
        drawfunction(postfix);
        deseneazaMeinu();
        handleMouse();

        //butoanele meniului



        if (GetKeyState(VK_OEM_PLUS) & 0x8000) {
            Zoom(1);
        }
        else if (GetKeyState(VK_OEM_MINUS) & 0x8000) {
            Zoom(0);
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            // Transformă în coordonate carteziene
            pair<double, double> coord = ConversieCoord(x, y);

            // Marchează punctul pe ecran
            setcolor(RED);
            circle(x, y, 5);

            // Afișează coordonatele în consolă
            cout << "Coordonate carteziene: (" << coord.first << ", " << coord.second << ")" << endl;
        }


        int CameraMove = 1000 / SCALE;
        if (GetKeyState(VK_UP) & 0x8000)
            CAMERA_Y += CameraMove;
        if (GetKeyState(VK_DOWN) & 0x8000)
            CAMERA_Y -= CameraMove;
        if (GetKeyState(VK_RIGHT) & 0x8000)
            CAMERA_X -= CameraMove;
        if (GetKeyState(VK_LEFT) & 0x8000)
            CAMERA_X += CameraMove;
        if (GetKeyState(VK_CONTROL) & 0x8000) {
            CAMERA_X = 0;
            CAMERA_Y = 0;
            SCALE = 50;
        }

        aP = 1 - aP;
    } while (!(GetKeyState(VK_ESCAPE) & 0x8000));

    closegraph();
    return 0;
}