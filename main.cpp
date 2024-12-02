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

void drawAxis() {
	setcolor(BLACK);
	line(SCREEN_ORIGIN_X, 0, SCREEN_ORIGIN_X, HEIGHT);
	line(0, SCREEN_ORIGIN_Y, WIDTH, SCREEN_ORIGIN_Y);
	for (int i = SCREEN_ORIGIN_X; i < WIDTH; i += scale) {
		line(i, SCREEN_ORIGIN_Y - 25*scale/100, i, SCREEN_ORIGIN_Y + 25*scale/100);
	}
	for (int i = SCREEN_ORIGIN_X; i > 0; i -= scale) {
		line(i, SCREEN_ORIGIN_Y - 25*scale/100, i, SCREEN_ORIGIN_Y + 25*scale/100);
	}
	for (int i = SCREEN_ORIGIN_Y; i < HEIGHT; i += scale) {
		line(SCREEN_ORIGIN_X - 25*scale/100, i, SCREEN_ORIGIN_X + 25*scale/100, i);
	}
	for (int i = SCREEN_ORIGIN_Y; i > 0; i -= scale) {
		line(SCREEN_ORIGIN_X - 25*scale/100, i, SCREEN_ORIGIN_X + 25*scale/100, i);
	}
}

//computarea de functii

double sinFunc(double x) { return sin(x); }
double cosFunc(double x) { return cos(x); }
double lnFunc(double x) { return log(x); }
double sqrtFunc(double x) { return sqrt(x); }

map<string, function<double(double)>> functions = {
    {"sin", sinFunc},
    {"cos", cosFunc},
    {"ln", lnFunc},
    {"sqrt", sqrtFunc}
};

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^";
}

bool isFunction(const string& token) {
    return functions.find(token) != functions.end();
}


int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

vector<string> infixToPostfix(const string& expression) {
    stack<string> operators;
    vector<string> output;
    stringstream ss(expression);
    string token;

    while (ss >> token) {
        if (isdigit(token[0]) || token == "x") {
            output.push_back(token); // Operanzi
        }
        else if (isFunction(token)) {
            operators.push(token); // Funcții
        }
        else if (token == "(") {
            operators.push(token);
        }
        else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.pop(); // Scoate "("
            if (!operators.empty() && isFunction(operators.top())) {
                output.push_back(operators.top());
                operators.pop();
            }
        }
        else if (isOperator(token)) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token)) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        }
    }

    while (!operators.empty()) {
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}


double evaluatePostfix(const vector<string>& postfix, double xValue) {
    stack<double> evalStack;

    for (const string& token : postfix) {
        if (isdigit(token[0])) { // Este număr
            evalStack.push(stod(token));
        }
        else if (token == "x") { // Este variabila x
            evalStack.push(xValue);
        }
        else if (isFunction(token)) { // Este funcție
            double operand = evalStack.top();
            evalStack.pop();
            evalStack.push(functions[token](operand));
        }
        else if (isOperator(token)) { // Este operator
            double right = evalStack.top(); evalStack.pop();
            double left = evalStack.top(); evalStack.pop();
            if (token == "+") evalStack.push(left + right);
            if (token == "-") evalStack.push(left - right);
            if (token == "*") evalStack.push(left * right);
            if (token == "/") evalStack.push(left / right);
            if (token == "^") evalStack.push(pow(left, right));
        }
    }

    return evalStack.top();
}


void drawfunction(vector<string> postfix)
{
    for (double X_POZ = 0;X_POZ < WIDTH;X_POZ+=1) {
        double Y_POZ = evaluatePostfix(postfix,(X_POZ-WIDTH/2)/WIDTH*(2*WIDTH/scale));
        
        int x = (int)(X_POZ), y = (int)(-Y_POZ*(scale/2));

        putpixel(x,y+HEIGHT/2, RED);
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

ORIGIN_Y = SCREEN_ORIGIN_Y+CAMERA_Y;
ORIGIN_X = SCREEN_ORIGIN_X+CAMERA_X;

    fereastra();
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

    aP = 1 - aP;
    } while (!(GetKeyState(VK_ESCAPE) & 0x8000));

    closegraph();
    return 0;
}
