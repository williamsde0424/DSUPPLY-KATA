/*
 * main.c
 *
 *  Created on: Dec 8, 2024
 *      Author: Demarion Williams
 *  Mission: Answer tough questions to escape the Gulag.
 *           Answer 3 correctly to earn your freedom.
 */
#include <windows.h>
#include <stdio.h>

typedef struct {
    const char *question;
    const char *answer_A;
    const char *answer_B;
    const char *answer_C;
    const char *answer_D;
    char correctAnswer;
} Question;

Question questions[4];
int currentQuestionIndex = 0;
int correctAnswers = 0;
HWND hwndScore, hwndResult, hwndQuestion, hwndMissionStatus, hwndTimer;
int timeLeft = 30;
int introTimerActive = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void loadQuestions(void);
void updateUI(HWND hwnd);
void updateMissionStatus(HWND hwnd);
void showIntroMessage(HWND hwnd);
void startIntroTimer(HWND hwnd);
void updateIntroTimer(HWND hwnd);
void startTimer(HWND hwnd);
void stopTimer(HWND hwnd);
void updateTimer(HWND hwnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    loadQuestions();

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "GulagEscapeApp";
    wc.hInstance = hInstance;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, "GulagEscapeApp", "Escape The Gulag: Tactical Questions", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    hwndScore = CreateWindow("STATIC", "Score: 0", WS_VISIBLE | WS_CHILD, 20, 20, 400, 40, hwnd, NULL, hInstance, NULL);
    hwndResult = CreateWindow("STATIC", "Your Fate...", WS_VISIBLE | WS_CHILD, 20, 80, 760, 80, hwnd, NULL, hInstance, NULL);
    hwndQuestion = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD, 20, 160, 760, 40, hwnd, NULL, hInstance, NULL);
    hwndMissionStatus = CreateWindow("STATIC", "Mission Status: Ready", WS_VISIBLE | WS_CHILD, 20, 500, 760, 40, hwnd, NULL, hInstance, NULL);
    hwndTimer = CreateWindow("STATIC", "Time Left: 30", WS_VISIBLE | WS_CHILD, 20, 440, 400, 40, hwnd, NULL, hInstance, NULL);

    CreateWindow("BUTTON", "A", WS_VISIBLE | WS_CHILD, 20, 220, 180, 60, hwnd, (HMENU)1, hInstance, NULL);
    CreateWindow("BUTTON", "B", WS_VISIBLE | WS_CHILD, 220, 220, 180, 60, hwnd, (HMENU)2, hInstance, NULL);
    CreateWindow("BUTTON", "C", WS_VISIBLE | WS_CHILD, 420, 220, 180, 60, hwnd, (HMENU)3, hInstance, NULL);
    CreateWindow("BUTTON", "D", WS_VISIBLE | WS_CHILD, 620, 220, 180, 60, hwnd, (HMENU)4, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    showIntroMessage(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            int buttonID = LOWORD(wParam);
            const char *selectedAnswer = "";
            switch (buttonID) {
            case 1: selectedAnswer = "A"; break;
            case 2: selectedAnswer = "B"; break;
            case 3: selectedAnswer = "C"; break;
            case 4: selectedAnswer = "D"; break;
            }

            stopTimer(hwnd);

            if (selectedAnswer[0] == questions[currentQuestionIndex].correctAnswer) {
                correctAnswers++;
                SetWindowText(hwndResult, "Correct! Keep fighting for freedom, soldier.");
            } else {
                SetWindowText(hwndResult, "Wrong. The guards are watching. Stay sharp!");
            }

            currentQuestionIndex++;
            if (correctAnswers >= 3) {
                char resultText[100];
                snprintf(resultText, sizeof(resultText), "Congratulations, you earned your freedom! Total Score: %d", correctAnswers);
                SetWindowText(hwndResult, resultText);
                SetWindowText(hwndMissionStatus, "Mission Complete. You are free!");
            } else if (currentQuestionIndex >= 4) {
                SetWindowText(hwndResult, "You've failed. You're thrown into the bottom of the gulag.");
                SetWindowText(hwndMissionStatus, "Game Over. You're stuck in the gulag...");
            } else {
                updateUI(hwnd);
                updateMissionStatus(hwnd);
                startTimer(hwnd);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_TIMER:
        if (introTimerActive) {
            updateIntroTimer(hwnd);
        } else {
            updateTimer(hwnd);
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void loadQuestions() {
    questions[0] = (Question){"What year did the first manned moon landing occur?", "1965", "1969", "1972", "1981", 'B'};
    questions[1] = (Question){"Which city is known as the birthplace of the Olympic Games?", "Rome", "Athens", "Paris", "Berlin", 'B'};
    questions[2] = (Question){"Who invented the telephone?", "Nikola Tesla", "Thomas Edison", "Alexander Graham Bell", "Albert Einstein", 'C'};
    questions[3] = (Question){"In which country was the first World War fought?", "USA", "Germany", "France", "Austria-Hungary", 'D'};
}

void updateUI(HWND hwnd) {
    SetWindowText(hwndQuestion, questions[currentQuestionIndex].question);
    SetWindowText(GetDlgItem(hwnd, 1), questions[currentQuestionIndex].answer_A);
    SetWindowText(GetDlgItem(hwnd, 2), questions[currentQuestionIndex].answer_B);
    SetWindowText(GetDlgItem(hwnd, 3), questions[currentQuestionIndex].answer_C);
    SetWindowText(GetDlgItem(hwnd, 4), questions[currentQuestionIndex].answer_D);

    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", correctAnswers);
    SetWindowText(hwndScore, scoreText);
}

void updateMissionStatus(HWND hwnd) {
    char status[100];
    snprintf(status, sizeof(status), "Mission Status: Question %d", currentQuestionIndex + 1);
    SetWindowText(hwndMissionStatus, status);
}

void showIntroMessage(HWND hwnd) {
    SetWindowText(hwndResult, "You have been captured and thrown into a remote gulag.");
    SetWindowText(hwndMissionStatus, "To escape, you must answer 3 tough questions correctly.");
    SetWindowText(hwndQuestion, "Prepare yourself. The first question is about to begin...");

    ShowWindow(GetDlgItem(hwnd, 1), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 2), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 3), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, 4), SW_HIDE);

    startIntroTimer(hwnd);
}

void startIntroTimer(HWND hwnd) {
    introTimerActive = 1;
    timeLeft = 5;
    char timerText[50];
    snprintf(timerText, sizeof(timerText), "Time Left: %d", timeLeft);
    SetWindowText(hwndTimer, timerText);

    SetTimer(hwnd, 1, 1000, NULL);
}

void updateIntroTimer(HWND hwnd) {
    if (timeLeft > 0) {
        timeLeft--;
    } else {
        SetWindowText(hwndResult, "First question starting...");
        SetWindowText(hwndMissionStatus, "Mission Status: Ready");

        ShowWindow(GetDlgItem(hwnd, 1), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, 2), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, 3), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, 4), SW_SHOW);

        currentQuestionIndex = 0;
        correctAnswers = 0;
        updateUI(hwnd);
        updateMissionStatus(hwnd);
        startTimer(hwnd);

        introTimerActive = 0;
    }

    char timerText[50];
    snprintf(timerText, sizeof(timerText), "Time Left: %d", timeLeft);
    SetWindowText(hwndTimer, timerText);
}

void startTimer(HWND hwnd) {
    timeLeft = 30;
    char timerText[50];
    snprintf(timerText, sizeof(timerText), "Time Left: %d", timeLeft);
    SetWindowText(hwndTimer, timerText);

    SetTimer(hwnd, 1, 1000, NULL);
}

void stopTimer(HWND hwnd) {
    KillTimer(hwnd, 1);
}

void updateTimer(HWND hwnd) {
    if (timeLeft > 0) {
        timeLeft--;
    } else {
        if (currentQuestionIndex == 0) {
            SetWindowText(hwndResult, "Time's up! You didn't answer in time.");
        } else {
            SetWindowText(hwndResult, "Time's up! The gulag guards are getting impatient.");
        }
        stopTimer(hwnd);
        currentQuestionIndex++;

        if (currentQuestionIndex >= 4) {
            SetWindowText(hwndResult, "Game Over. You're stuck in the gulag...");
            SetWindowText(hwndMissionStatus, "Game Over.");
        } else {
            updateUI(hwnd);
            updateMissionStatus(hwnd);
            startTimer(hwnd);
        }
    }

    char timerText[50];
    snprintf(timerText, sizeof(timerText), "Time Left: %d", timeLeft);
    SetWindowText(hwndTimer, timerText);
}
