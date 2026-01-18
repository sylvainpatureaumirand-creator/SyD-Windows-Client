/*

gcc -municode ressources.o syd-v0.2.c -o "C:\Users\MOI\SYD\builds\Test\SyD.exe" -I"C:/Users/MOI/SYD/include" -L"." -lsyd -lshlwapi -lcomdlg32 -mwindows

*/

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <shellapi.h>  // Pour CommandLineToArgvW
#include <shlwapi.h>   // Pour PathRemoveFileSpecW 
#include "C:/Users/MOI/SYD/include/SYD.h"


#define MN_SYDCLIENT  11
#define MN_CLOSE      19
#define BTN_SAVE      21
#define BTN_CLOSE     29
//#define IDI_ICON1     999

// Handle
HANDLE hExplorerProcess = NULL; // à stocker quelque part globalement si besoin
// Declaration de la police
HFONT hFont;

// Déclaration des variables globales
wchar_t DIRECTORY_PATH[MAX_PATH] = L"C:\\Users\\Public\\Documents";
wchar_t BDD_PATH[MAX_PATH];
wchar_t CONF_PATH[MAX_PATH];
wchar_t TMP_PATH[MAX_PATH];
wchar_t TMP_REP_PATH[MAX_PATH];
wchar_t monUUID_PATH[MAX_PATH];
char OPENED_SYD[13];
char OPENED_FILE[MAX_PATH];
wchar_t SHOW_OPENED_FILE[MAX_PATH];
// Déclaration des structures

typedef struct {
    char uuid[40];
    char ip[20];
    char port[10];
} ConfigSVR;

// Fonctions de traitement

void OpenGestionnaire(const wchar_t* folderPath, HWND hWnd) {
    // Vérifie si le dossier existe
    DWORD attribs = GetFileAttributesW(folderPath);
        if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Le dossier n'existe pas : %s", folderPath);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
        return;
    }
    wchar_t gestionnaire_path[MAX_PATH];
    swprintf(gestionnaire_path, MAX_PATH, L"%ls\\Gestionnaire.exe", (wchar_t*)folderPath);

    // Tente d'ouvrir avec ShellExecuteW
    HINSTANCE result = ShellExecuteW(
        NULL,                   // pas de fenêtre parente
        L"open",                // action
        gestionnaire_path,      // chemin du programme
        NULL,                   // paramètres
        NULL,                   // répertoire de travail
        SW_SHOWNORMAL           // mode d'affichage
    );

    // Vérifie le résultat
    if ((INT_PTR)result <= 32) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Échec de l'ouverture gestionnaire (code %ld).", (INT_PTR)result);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
    }
}

void OpenExplorerFolder(const wchar_t* folderPath, HWND hWnd) {
    DWORD attribs = GetFileAttributesW(folderPath);
    if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        wchar_t msg[128];
        swprintf(msg, 128, L"Le dossier n'existe pas : %ls", folderPath);
        MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
        return;
    }

    // Préparation de la commande
    wchar_t commandLine[MAX_PATH + 50];
    swprintf(commandLine, MAX_PATH + 50, L"explorer.exe \"%ls\"", folderPath);

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL success = CreateProcessW(
        NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
    );

    if (success) {
        hExplorerProcess = pi.hProcess; // stocker pour fermeture ultérieure
        CloseHandle(pi.hThread); // on n’a pas besoin du thread
    } else {
        MessageBoxW(hWnd, L"Impossible d'ouvrir l'explorateur.", L"Erreur", MB_ICONERROR);
    }
}

void FermerExplorateurOuvert() {
    if (hExplorerProcess) {
        TerminateProcess(hExplorerProcess, 0);  // fermeture brutale
        CloseHandle(hExplorerProcess);          // libère le handle
        hExplorerProcess = NULL;
    }
}

void OpenSYDFile(HWND hWnd, const wchar_t *file_Path) {
    char confAnsi[MAX_PATH], bddAnsi[MAX_PATH], tmpAnsi[MAX_PATH], tmpRepAnsi[MAX_PATH], fileAnsi[MAX_PATH];

    // Conversion des chemins wide en ANSI
    WideCharToMultiByte(CP_ACP, 0, CONF_PATH, -1, confAnsi, MAX_PATH, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, BDD_PATH, -1, bddAnsi, MAX_PATH, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, TMP_PATH, -1, tmpAnsi, MAX_PATH, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, file_Path, -1, fileAnsi, MAX_PATH, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, file_Path, -1, OPENED_FILE, MAX_PATH, NULL, NULL);

    printf(" |exe| |OpenSYDFile| Conf %s BDD %s Tmp %s\n",confAnsi, bddAnsi, tmpAnsi);
    printf(" |exe| |OpenSYDFile| File %s Opened %s\n",fileAnsi, OPENED_FILE);


    int ret_SYD_OpenSYD = SYD_OpenSYD(confAnsi, bddAnsi, tmpAnsi, fileAnsi, tmpRepAnsi);
    char tmpRepPathAnsi[550];
    snprintf(tmpRepPathAnsi, sizeof(tmpRepPathAnsi), "%s%s", tmpAnsi, tmpRepAnsi);
    memset(OPENED_SYD, '\0', sizeof(OPENED_SYD));
    snprintf(OPENED_SYD, sizeof(OPENED_SYD), "%s", tmpRepAnsi);
    MultiByteToWideChar(CP_ACP, 0, tmpRepPathAnsi, -1, TMP_REP_PATH, MAX_PATH);

    if (ret_SYD_OpenSYD != 0) { // Gestion des erreurs
            if (ret_SYD_OpenSYD == 10) {
                MessageBoxW(hWnd, L"Selectionnez un\n  fichier .syd", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else if (ret_SYD_OpenSYD == 20) {
                MessageBoxW(hWnd, L"Vous ne pouvez pas \ndéchiffrez ce fichier", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else {
                wchar_t msg[128];
                swprintf(msg, 128, L"L'ouverture a échoué\navec le code %d", ret_SYD_OpenSYD);
                MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
            }
    } else { // Ouveture de l'explorateur
        OpenExplorerFolder(TMP_REP_PATH, hWnd);
    }
}

void CloseSYDFile(HWND hWnd) {
    
    char confAnsi[MAX_PATH], bddAnsi[MAX_PATH], tmpAnsi[MAX_PATH], tmpRepAnsi[MAX_PATH], fileAnsi[MAX_PATH];
    
        // Conversion des chemins wide en ANSI
        WideCharToMultiByte(CP_ACP, 0, CONF_PATH, -1, confAnsi, MAX_PATH, NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, BDD_PATH, -1, bddAnsi, MAX_PATH, NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, TMP_PATH, -1, tmpAnsi, MAX_PATH, NULL, NULL);
        
        snprintf(fileAnsi, sizeof(fileAnsi), "%s%s.syd", tmpAnsi, OPENED_SYD);

        int ret_SYD_CloseSYD = SYD_CloseSYD(confAnsi, bddAnsi, tmpAnsi, OPENED_FILE, OPENED_SYD);
        memset(OPENED_SYD, '\0', sizeof(OPENED_SYD));
        // Affichage du retour
        if (ret_SYD_CloseSYD != 0) {// Gestion des erreurs
            if (ret_SYD_CloseSYD == 10) {
                MessageBoxW(hWnd, L"Le fichier SyD\nn'existe pas", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else if (ret_SYD_CloseSYD == 11) {
                MessageBoxW(hWnd, L"Le fichier est encore ouvert !\nSauvergardez le en dehors et réouvrez l'archive", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else if (ret_SYD_CloseSYD == 23) {
                MessageBoxW(hWnd, L"Le fichier est encore ouvert !\nSauvergardez le en dehors et réouvrez l'archive", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            }  else if (ret_SYD_CloseSYD == 24) {
                MessageBoxW(hWnd, L"Le fichier est encore ouvert !\nSauvergardez le en dehors et réouvrez l'archive", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else if (ret_SYD_CloseSYD == 44) {
                MessageBoxW(hWnd, L"Erreur de fermture de l'archive\nSauvergardez le fichier !", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else if (ret_SYD_CloseSYD == 20) {
                MessageBoxW(hWnd, L"Vous ne pouvez pas \nchiffrez ce fichier", L"Erreur", MB_ICONERROR);
                DestroyWindow(hWnd);
            } else {
                wchar_t msg[128];
                swprintf(msg, 128, L"L'enregistrement a échoué\navec le code %d", ret_SYD_CloseSYD);
                MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
            }
        } else { // Enregistrement réussi
            // Fermeture de l'explorateur
            FermerExplorateurOuvert();
            // Fermeture propre de l'application
            DestroyWindow(hWnd);
        }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam, RGB(40, 40, 40));
            SetBkColor((HDC)wParam, RGB(240, 240, 240));
            return (INT_PTR)CreateSolidBrush(RGB(240, 240, 240));

        case WM_CTLCOLORBTN:
            SetTextColor((HDC)wParam, RGB(0, 0, 0));
            SetBkColor((HDC)wParam, RGB(220, 220, 220));
            return (INT_PTR)CreateSolidBrush(RGB(220, 220, 220));


        case WM_CREATE: {

            HICON hIcon = (HICON)LoadImageW(GetModuleHandle(NULL),
                                MAKEINTRESOURCE(IDI_ICON1),
                                IMAGE_ICON,
                                0, 0,
                                LR_DEFAULTSIZE | LR_SHARED);

            SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

            hFont = CreateFontW(
                18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
            );

            HFONT hFontCali = CreateFontW(
                18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Calibri"
            );

            // Menu
            HMENU hMenubar = CreateMenu();
            HMENU hConfiguration = CreateMenu();
            AppendMenuW(hConfiguration, MF_STRING, MN_SYDCLIENT, L"Gestionnaire");
            AppendMenuW(hConfiguration, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hConfiguration, MF_STRING, MN_CLOSE, L"Quitter");
            AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hConfiguration, L"Menu");
            SetMenu(hWnd, hMenubar);

            // Titre principal
            HWND hTitle = CreateWindowW(L"STATIC", L"Conteneur SYD",
                WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
                20, 10, 200, 40, hWnd, NULL, NULL, NULL);
            SendMessageW(hTitle, WM_SETFONT, (WPARAM)hFont, TRUE);

            // Zone d'information
            HWND hInfo = CreateWindowW(L"STATIC", L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
                20, 60, 200, 20, hWnd, (HMENU)101, NULL, NULL);
            SendMessageW(hInfo, WM_SETFONT, (WPARAM)hFontCali, TRUE);
            SetWindowTextW(hInfo, SHOW_OPENED_FILE);

            // Bouton principal
            HWND hButton = CreateWindowW(L"BUTTON", L"Enregistrer",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER,
                70, 90, 100, 40, hWnd, (HMENU)BTN_SAVE, NULL, NULL);
            SendMessageW(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

            break;
        }

    case WM_DESTROY: 
    {
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);

        // Zone d'affectation du comportement de l'interface graphique
        case WM_COMMAND: 
        {
            switch (LOWORD(wParam)) {

                // Bouton sauvegarder le SYD
                case BTN_SAVE: {
                    Sleep(150);
                    CloseSYDFile(hWnd);
                    break;
                }
                
                case MN_SYDCLIENT: {
                    OpenGestionnaire(DIRECTORY_PATH, hWnd);
                    break;
                }

                case MN_CLOSE: {// Quitter depuis le menu "SYD"
                    DestroyWindow(hWnd);
                    break;
                }
                
            }
        } break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

// Déclaration de la classe de la fenetre principale
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"FenetreBoutons";
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    // Icônes système
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_ICON1));   // Coin supérieur gauche
    wc.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_ICON1));   // Alt+Tab et barre des tâches

    RegisterClassExW(&wc);


// Collecte du chemin du programme

    GetModuleFileNameW(NULL, DIRECTORY_PATH, MAX_PATH);
    PathRemoveFileSpecW(DIRECTORY_PATH);
    wchar_t cleanedPath[MAX_PATH];
    if (lpCmdLine && wcslen(lpCmdLine) > 0) {
    // Nettoyage des guillemets

        if (lpCmdLine[0] == L'"') {
            wcsncpy(cleanedPath, lpCmdLine + 1, MAX_PATH);
            size_t len = wcslen(cleanedPath);
            if (len > 0 && cleanedPath[len - 1] == L'"') {
                cleanedPath[len - 1] = L'\0';
            }
        } else {
            wcsncpy(cleanedPath, lpCmdLine, MAX_PATH);
        }
        wchar_t *filename = PathFindFileName(cleanedPath);
        wcsncpy(SHOW_OPENED_FILE, filename, MAX_PATH);
    }



    // MessageBoxW(NULL, DIRECTORY_PATH, L"Chemin répertoire", MB_OK);

// Creation des chemins des repertoires du client
    swprintf(BDD_PATH, MAX_PATH, L"%ls\\bdd\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(CONF_PATH, MAX_PATH, L"%ls\\conf\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(TMP_PATH, MAX_PATH, L"%ls\\tmp\\", (wchar_t*)DIRECTORY_PATH);

    HWND hwnd = CreateWindowExW(0, L"FenetreBoutons", L"SyD",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, 260, 200,
                            NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Si un chemin est passé en paramètre, on tente d'ouvrir le fichier

        // Appel de la fonction avec le chemin nettoyé
        OpenSYDFile(hwnd, cleanedPath);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;

    // Fin de programme
    //LocalFree(argv); // Libère la mémoire allouée par CommandLineToArgvW
    return 0;
}
