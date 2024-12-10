#include    "graphics.h"
#include    <iostream>
#include    <stack>
#include    <cmath>
#include    <string>
#include    <vector>
#include    <map>
#include    <Windows.h>
using namespace std;

// Dimensiunile ecranului grafic
int WIDTH = GetSystemMetrics(SM_CXSCREEN);
int HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int ORIGIN_X = WIDTH / 2;
int ORIGIN_Y = HEIGHT / 2;
int CAMERA_X = 0;
int CAMERA_Y = 0;
bool FULSCREEN = 1;
bool isDarkModeOn = 0;

int SCALE = 50;

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

void drawAxis(int culoare) {
    //axele de coordonate
    setfillstyle(SOLID_FILL, culoare);
    bar(ORIGIN_X + CAMERA_X, 0, ORIGIN_X + CAMERA_X+1, HEIGHT); //OY
    bar(0, ORIGIN_Y + CAMERA_Y, WIDTH, ORIGIN_Y + CAMERA_Y + 1); //OX

    int minX = (0 - ORIGIN_X - CAMERA_X) / SCALE; 
    int maxX = (WIDTH - ORIGIN_X - CAMERA_X) / SCALE; 
    int minY = (ORIGIN_Y + CAMERA_Y - HEIGHT) / SCALE; 
    int maxY = (ORIGIN_Y + CAMERA_Y) / SCALE; 

    for (int i = minX; i <= maxX;i++) {
        int x = ORIGIN_X + CAMERA_X + i * SCALE;
        bar(x, ORIGIN_Y + CAMERA_Y - 5, x+1, ORIGIN_Y + CAMERA_Y + 5);
    }
    for (int i = minY;i <= maxY; i++ ) {
        int y = ORIGIN_Y + CAMERA_Y - i * SCALE;
        bar(ORIGIN_X + CAMERA_X  - 5,y, ORIGIN_X + CAMERA_X + 5, y+1);
    }

}

void drawfunction(vector<string> postfix) //adaugare parametru de culoare pt a desena mai multe functii simultan
{
    double PreviousY = evaluareExpresie(postfix, (0 - ORIGIN_X - CAMERA_X) / SCALE);
    int prev_y = (int)(PreviousY * SCALE);
    prev_y = -prev_y + HEIGHT / 2 + CAMERA_Y;
    putpixel(0, prev_y+HEIGHT/2+CAMERA_Y,RED);

    for (double X_POZ = 1;X_POZ < WIDTH;X_POZ+=1) {
        double Y_POZ = evaluareExpresie(postfix, (X_POZ - ORIGIN_X - CAMERA_X)/SCALE); // modific cu coord lui x
        
        int x = (int)(X_POZ), y = (int)(Y_POZ*SCALE);
        y = -y + HEIGHT / 2 + CAMERA_Y; //conversia pt a plasa punctul corect pe ecran, deoarece originea ecranului este in coltul stanga-sus

        //fix pentru afisarea graficelor punctat

        if (abs(y - prev_y) > 2 && abs(y-prev_y) < 10000) {   
            setfillstyle(SOLID_FILL, RED);
            bar(x - 1, y, x, prev_y);
        }
        else
            putpixel(x,y, RED);
        prev_y = y;
        //cout << x << " " << y << '\n';
    }
}

void DarkMode(int &fundal, int &axe)
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

void fereastra()
{
    if (FULSCREEN) {
        WIDTH = GetSystemMetrics(SM_CXSCREEN);
        HEIGHT = GetSystemMetrics(SM_CYSCREEN);
        closegraph();
        ORIGIN_Y = HEIGHT / 2;
        ORIGIN_X = WIDTH / 2;
        initwindow(WIDTH, HEIGHT, "", -3, -3);
        delay(100);
    }else {
        WIDTH = 1600;HEIGHT = 1200;
        closegraph();
        ORIGIN_Y = HEIGHT / 2;
        ORIGIN_X = WIDTH / 2;
        initwindow(WIDTH, HEIGHT, ".");
        delay(100);
    }

}

bool scroll(int up_or_down)
{
    GET_WHEEL_DELTA_WPARAM(up_or_down);
    return up_or_down;
}

int main() {
int aP = 0;
    //int gd = DETECT, gm=DETECT;
string expression;
cout << "Introdu expresia matematica: ";
getline(cin, expression);
cout << expression;
vector<string> postfix = infixToPostfix(expression);

for (auto i : postfix)
    cout << i << " ";



    int culoareFundal, culoareAxe;
    DarkMode(culoareFundal, culoareAxe);
    fereastra();
    drawAxis(culoareAxe);
   // drawfunction2(postfix);
    do {
        //trecerea ferestrei din windowed in fulscreen
        if (GetKeyState(VK_F11) & 0x8000) {
            FULSCREEN = !FULSCREEN;
            fereastra();
        }
        setactivepage(aP);
		setvisualpage(1 - aP);
        cleardevice();
//        if (ismouseclick(WM_RBUTTONDOWN))
//            isDarkModeOn = !isDarkModeOn;
        DarkMode(culoareFundal, culoareAxe);
        setbkcolor(culoareFundal);
		drawAxis(culoareAxe);
        drawfunction(postfix);
        if (GetKeyState(VK_OEM_PLUS) & 0x8000) {
            if (SCALE >= 200 && SCALE <= 500) {
                SCALE += 25;
                delay(10);
            }
            else {
                SCALE += 10;
                delay(10);
            }
        }
        else if (GetKeyState(VK_OEM_MINUS) & 0x8000) {
            if (SCALE >25) {
                if (SCALE >= 200) {
                    SCALE -= 25;
                }
                else {
                    SCALE -= 10;
                }
            }
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
        int CameraMove = 2*SCALE;
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
        if (GetKeyState(VK_F7) & 0x8000) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);
            double X = (double)x;
            cout << (X - WIDTH / 2) / WIDTH * (WIDTH / SCALE) << '\n';
        }

    aP = 1 - aP;
    } while (!(GetKeyState(VK_ESCAPE) & 0x8000));

    closegraph();
    return 0;
}
