/*

gcc -municode ressources.o Annuaire-v0.2.c -o "C:\Users\MOI\SYD\builds\Test\Annuaire.exe" -I"C:/Users/MOI/SYD/include" -L"." -lsyd -lshlwapi -lcomdlg32 -mwindows -lgdi32

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

#define MAX_LIGNES 100

#define BTN_ADD      2
#define BTN_DELETE   3
#define BTN_REFRESH  7
#define EDIT_TYPE    1001
#define EDIT_UUID    1002
#define EDIT_NOM     1003
#define LISTBOX_ID   2001
//#define IDI_ICON1     999

// Handle
// HANDLE hExplorerProcess = NULL; // à stocker quelque part globalement si besoin
// Declaration de la police
HFONT hFont;

// Déclaration des variables globales
wchar_t DIRECTORY_PATH[MAX_PATH] = L"C:\\Users\\Public\\Documents";
wchar_t BDD_PATH[MAX_PATH];
wchar_t CONF_PATH[MAX_PATH];
wchar_t TMP_PATH[MAX_PATH];
wchar_t TMP_REP_PATH[MAX_PATH];
wchar_t monUUID_PATH[MAX_PATH];
wchar_t monUUID[64];
char OPENED_SYD[13];
char OPENED_FILE[MAX_PATH];
// Déclaration des structures

typedef struct {
    char uuid[40];
    char ip[20];
    char port[10];
} ConfigSVR;

typedef struct {
    char type[40];
    char uuid2[40];
    char texte[256];
} LigneCSV;

// Variables globales
LigneCSV lignes[MAX_LIGNES];
int nbLignes = 0;
HWND hListBox;

// Fonctions de traitement

void ChargerCSV(const char *chemin) {
    FILE *f = fopen(chemin, "r");
    if (!f) return;
    nbLignes = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), f) && nbLignes < MAX_LIGNES) {
        sscanf(buffer, "%63[^;];%63[^;];%255[^\n]",
               lignes[nbLignes].type,
               lignes[nbLignes].uuid2,
               lignes[nbLignes].texte);
        nbLignes++;
    }
    fclose(f);
}

void RemplirListBox() {
    SendMessageW(hListBox, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < nbLignes; i++) {
        wchar_t texteW[256];
        MultiByteToWideChar(CP_UTF8, 0, lignes[i].texte, -1, texteW, 256);
        SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)texteW);
    }
}

void AjouterLigne(const char *type, const char *uuid2, const char *texte) {
    FILE *f = fopen("annuaire.csv", "a");
    if (f) {
        fprintf(f, "%s;%s;%s\n", type, uuid2, texte);
        fclose(f);
    }
    ChargerCSV("annuaire.csv");
    RemplirListBox();
}

void SupprimerLigne(int index) {
    FILE *f = fopen("annuaire.csv", "w");
    if (!f) return;
    for (int i = 0; i < nbLignes; i++) {
        if (i != index) {
            fprintf(f, "%s;%s;%s\n", lignes[i].type, lignes[i].uuid2, lignes[i].texte);
        }
    }
    fclose(f);
    ChargerCSV("annuaire.csv");
    RemplirListBox();
}

BOOL ChargerConfig(const wchar_t *cheminFichier, ConfigSVR *config) {
    FILE *f = _wfopen(cheminFichier, L"rb");  // Lecture en binaire (ANSI/UTF-8)
    if (!f) return FALSE;

    char ligneAnsi[512];
    wchar_t ligne[512];

    while (fgets(ligneAnsi, sizeof(ligneAnsi), f)) {
        // Conversion UTF-8 → UTF-16
        MultiByteToWideChar(CP_UTF8, 0, ligneAnsi, -1, ligne, 512);

        // Extraction des champs JSON
        if (wcsstr(ligne, L"\"uuid\"")) {
            swscanf(ligne, L" \"uuid\" : \"%63[^\"]\"", config->uuid);
        } else if (wcsstr(ligne, L"\"IP\"")) {
            swscanf(ligne, L" \"IP\" : \"%63[^\"]\"", config->ip);
        } else if (wcsstr(ligne, L"\"PORT\"")) {
            swscanf(ligne, L" \"PORT\" : \"%15[^\"]\"", config->port);
        }
    }

    fclose(f);
    return TRUE;
}

BOOL ChargerMonUUID(const wchar_t *cheminFichier, wchar_t *monuuidw) {
    FILE *f = _wfopen(cheminFichier, L"rt, ccs=UTF-8");  // Lecture du texte avec encodage UTF-8
    if (!f) return FALSE;

    wchar_t ligne[256];
    monuuidw[0] = L'\0';  // Initialisation du buffer de sortie

    while (fgetws(ligne, sizeof(ligne) / sizeof(wchar_t), f)) {
        // Nettoye fin de ligne
        size_t len = wcslen(ligne);
        if (len > 0 && (ligne[len - 1] == L'\n' || ligne[len - 1] == L'\r')) {
            ligne[len - 1] = L'\0';
        }

        // Cherche la clé "UUID"
        if (wcsstr(ligne, L"\"UUID\"")) {
            // Extraction manuelle entre guillemets
            wchar_t *after_colon = wcschr(ligne, L':');
            if (after_colon) {
                wchar_t *start = wcschr(after_colon, L'"');
                if (start) {
                    start++;  // avancer après le guillemet
                    wchar_t *end = wcschr(start, L'"');
                    if (end && (end - start) == 36) {  // UUID standard = 36 caractères
                        wcsncpy(monuuidw, start, 36);
                        monuuidw[36] = L'\0';  // terminaison explicite
                        fclose(f);
                        return TRUE;  // Extraction réussie
                    }
                }
            }
        }
    }

    fclose(f);
    return FALSE;  // UUID non trouvé ou mal formé
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
            // Bandeau Entete
            CreateWindowW(L"STATIC", L"Annuaire", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 20, 10, 600, 40, hWnd, NULL, NULL, NULL);

            // Zone d'affichage de l'annuaire
            // ListBox
            hListBox = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_EXTENDEDSEL | WS_VSCROLL, 20, 60, 250, 220, hWnd, (HMENU)LISTBOX_ID, NULL, NULL);
            CreateWindowW(L"STATIC", L"Selectionnez pour", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 20, 280, 160, 25, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Supprimer", WS_VISIBLE | WS_CHILD , 190, 280, 80, 25, hWnd, (HMENU)BTN_DELETE, NULL, NULL);

            // Zones de saisie 
            CreateWindowW(L"STATIC", L"Ajouter un identifiant SYD", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 290, 60, 330, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"STATIC", L"Type", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 290, 100, 40, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 340, 100, 280, 20, hWnd, (HMENU)EDIT_TYPE, NULL, NULL);

            CreateWindowW(L"STATIC", L"Ident.", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 290, 130, 40, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 340, 130, 280, 20, hWnd, (HMENU)EDIT_UUID, NULL, NULL);

            CreateWindowW(L"STATIC", L"Nom", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 290, 160, 40, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 340, 160, 280, 20, hWnd, (HMENU)EDIT_NOM, NULL, NULL);

            CreateWindowW(L"BUTTON", L"Ajouter", WS_VISIBLE | WS_CHILD | BS_CENTER , 420, 190, 60, 25, hWnd, (HMENU)BTN_ADD, NULL, NULL);
            
            // Zone affichage de mon identifiant
            CreateWindowW(L"STATIC", L"Mon Identifiant", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 290, 250, 330, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER | SS_CENTERIMAGE, 290, 280, 330, 20, hWnd, (HMENU)401, NULL, NULL);       
            
            
            // Bouton de fermeture de la fenetre
            CreateWindowW(L"BUTTON", L"Fermer", WS_VISIBLE | WS_CHILD, 260, 320, 80, 25, hWnd, (HMENU)BTN_CLOSE, NULL, NULL);
            
            // Fonctions
            ChargerCSV("annuaire.csv");
            RemplirListBox();
   
            // Declaration des variables
            wchar_t syduuidW[40];
            ConfigSVR sydcfg;
            ConfigSVR spcfg;
            // Charger le fichier de config existant
            ChargerConfig(L"conf\\ServerSYD.cfg", &sydcfg);
            // Creation du chemin de lecture de mon UUID en fonction du SYD

            MultiByteToWideChar(CP_UTF8, 0, sydcfg.uuid, -1, syduuidW, 40);
            swprintf(monUUID_PATH, MAX_PATH, L"conf\\%ls.CL", syduuidW);
            //wprintf(L"SYD UUID : %s\n", sydcfg.uuid);
            //wprintf(L"SYD UUID : %ls\n", syduuidW);
            //wprintf(L"PATH UUID %ls\n", monUUID_PATH);
            
            if ( ChargerMonUUID(monUUID_PATH, monUUID)) {
                SetWindowTextW(GetDlgItem(hWnd, 401), monUUID);
            }     
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {

                // Bouton Ajouter
                case BTN_ADD: {
                    wchar_t uuid1W[64], uuid2W[64], texteW[256];
                    char type[64], uuid2[64], texte[256];

                    // Récupérer les chaînes en Unicode (UTF-16)
                    GetWindowTextW(GetDlgItem(hWnd, EDIT_TYPE), uuid1W, 64);
                    GetWindowTextW(GetDlgItem(hWnd, EDIT_UUID), uuid2W, 64);
                    GetWindowTextW(GetDlgItem(hWnd, EDIT_NOM), texteW, 256);

                    // Convertir en ANSI (UTF-8 si besoin) pour la fonction AjouterLigne
                    WideCharToMultiByte(CP_ACP, 0, uuid1W, -1, type, 64, NULL, NULL);
                    WideCharToMultiByte(CP_ACP, 0, uuid2W, -1, uuid2, 64, NULL, NULL);
                    WideCharToMultiByte(CP_ACP, 0, texteW, -1, texte, 256, NULL, NULL);

                    if (wcslen(uuid1W) && wcslen(uuid2W) && wcslen(texteW)) {
                        AjouterLigne(type, uuid2, texte);

                        // Réinitialiser les champs (Unicode)
                        SetWindowTextW(GetDlgItem(hWnd, EDIT_TYPE), L"");
                        SetWindowTextW(GetDlgItem(hWnd, EDIT_UUID), L"");
                        SetWindowTextW(GetDlgItem(hWnd, EDIT_NOM), L"");
                    } else {
                        MessageBoxW(hWnd, L"Tous les champs doivent être remplis.", L"Erreur", MB_ICONERROR);
                    }
                    ChargerCSV("annuaire.csv");
                    RemplirListBox();

                    break;
                }
                
                // Bouton effacer
                case BTN_DELETE: {
                    int index = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                    if (index != LB_ERR) {
                        SupprimerLigne(index);
                        }
                        ChargerCSV("annuaire.csv");
                        RemplirListBox();
                        
                    break;
                }
                
                case BTN_CLOSE: { // ID du bouton "Fermer"
                DestroyWindow(hWnd);  // Ferme la fenêtre
                break;
                }
        }
        break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
        break;

        case WM_DESTROY:
            PostQuitMessage(0); // Signale à la boucle de messages de s'arrêter
        return 0;


        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
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

// Creation des chemins des repertoires du client
    swprintf(BDD_PATH, MAX_PATH, L"%ls\\bdd\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(CONF_PATH, MAX_PATH, L"%ls\\conf\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(TMP_PATH, MAX_PATH, L"%ls\\tmp\\", (wchar_t*)DIRECTORY_PATH);

    HWND hwnd = CreateWindowExW(0, L"FenetreBoutons", L"Annuaire",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, 660, 400,
                            NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

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
