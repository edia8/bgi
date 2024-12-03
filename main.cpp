#include    "graphics.h"
#include    <iostream>
#include    <stack>
#include    <cmath>
#include    <string>
#include    <vector>
#include    <sstream>
#include    <map>
#include    <functional>
#include    <Windows.h>

using namespace std;

// Dimensiunile ecranului grafic
int WIDTH = GetSystemMetrics(SM_CXSCREEN);
int HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int SCREEN_ORIGIN_X = WIDTH / 2;
int SCREEN_ORIGIN_Y = HEIGHT / 2;
int ORIGIN_X = SCREEN_ORIGIN_X;
int ORIGIN_Y = SCREEN_ORIGIN_Y;
int CAMERA_X = 0;
int CAMERA_Y = 0;
bool FULSCREEN = 1;

int scale = 100;

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

void drawAxis() {
    setcolor(BLACK);
    line(ORIGIN_X, 0, ORIGIN_X, HEIGHT);
    line(0, ORIGIN_Y, WIDTH, ORIGIN_Y);
    for (int i = ORIGIN_X; i < WIDTH; i += scale) {
        line(i, ORIGIN_Y - 25 * scale / 100, i, ORIGIN_Y + 25 * scale / 100);
    }
    for (int i = ORIGIN_X; i > 0; i -= scale) {
        line(i, ORIGIN_Y - 25 * scale / 100, i, ORIGIN_Y + 25 * scale / 100);
    }
    for (int i = ORIGIN_Y; i < HEIGHT; i += scale) {
        line(ORIGIN_X - 25 * scale / 100, i, ORIGIN_X + 25 * scale / 100, i);
    }
    for (int i = ORIGIN_Y; i > 0; i -= scale) {
        line(ORIGIN_X - 25 * scale / 100, i, ORIGIN_X + 25 * scale / 100, i);
    }
}

void drawfunction(vector<string> postfix)
{
    for (double X_POZ = 0;X_POZ < WIDTH;X_POZ+=1) {
        double Y_POZ = evaluareExpresie(postfix,(X_POZ-WIDTH/2)/WIDTH*(WIDTH/scale));
        
        int x = (int)(X_POZ), y = (int)(-Y_POZ*scale);

        putpixel(x,y+HEIGHT/2, RED);
        //cout << x << " " << y << '\n';
    }
}
void drawfunction2(vector<string> postfix)//verificarea de valori ale functiei(pt debug)
{
    for (double X_POZ = 0;X_POZ < WIDTH;X_POZ += 1) {
        double Y_POZ = evaluareExpresie(postfix, (X_POZ - WIDTH / 2) / WIDTH * ( WIDTH / scale));

        int x = (int)(X_POZ), y = (int)(-Y_POZ * scale);

        putpixel(x, y + HEIGHT / 2, RED);
        cout << x << " " << y << '\n';
    }
}

void fereastra()
{
    if (FULSCREEN) {
        WIDTH = GetSystemMetrics(SM_CXSCREEN);
        HEIGHT = GetSystemMetrics(SM_CYSCREEN);
        closegraph();
        SCREEN_ORIGIN_Y = HEIGHT / 2;
        SCREEN_ORIGIN_X = WIDTH / 2;
        initwindow(WIDTH, HEIGHT, "", -3, -3);
        delay(100);
    }else {
        WIDTH = 1600;HEIGHT = 1200;
        closegraph();
        SCREEN_ORIGIN_Y = HEIGHT / 2;
        SCREEN_ORIGIN_X = WIDTH / 2;
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



    fereastra();
    drawAxis();
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
        setbkcolor(WHITE);
        SCREEN_ORIGIN_Y = ORIGIN_Y + CAMERA_Y;
        SCREEN_ORIGIN_X = ORIGIN_X + CAMERA_X;
		drawAxis();
        drawfunction(postfix);
        if (GetKeyState(VK_OEM_PLUS) & 0x8000) {
            if (scale >= 200 && scale<=500)
                scale += 50;
            else
                scale += 10;
        }
        else if (GetKeyState(VK_OEM_MINUS) & 0x8000) {
            if (scale - 10 != 0) {
                if (scale >= 200)
                    scale -= 50;
                else
                    scale -= 10;
            }
        }
        if (GetKeyState(VK_UP) & 0x8000)
            CAMERA_X += 10;

    aP = 1 - aP;
    } while (!(GetKeyState(VK_ESCAPE) & 0x8000));

    closegraph();
    return 0;
}
