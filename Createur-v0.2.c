/*

gcc -municode ressources.o Createur-v0.2.c -o "C:\Users\MOI\SYD\builds\Test\Createur.exe" -I"C:/Users/MOI/SYD/include" -L"." -lsyd -lshlwapi -lcomdlg32 -mwindows
Pour compiler en mode debug
gcc -municode ressources.o Createur-v0.2.c -o "C:\Users\MOI\SYD\builds\Test\Createur.exe" -I"C:/Users/MOI/SYD/include" -L"." -lsyd -lshlwapi -lcomdlg32 -lgdi32 -lcomctl32
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

#define MAX_LIGNES 100
#define BTN_INIT     1
#define BTN_ADD      2
#define BTN_DELETE   3
#define BTN_ASK      4
#define BTN_OPEN     5
#define BTN_SYD      6
#define BTN_REFRESH  7
#define BTN_CLOSE    9
#define EDIT_TYPE    1001
#define EDIT_UUID    1002
#define EDIT_NOM     1003
#define LISTBOX_ID   2001
#define COMBOBOX1_ID  2002
#define COMBOBOX2_ID  2003

// Handle
HANDLE hExplorerProcess = NULL; // à stocker quelque part globalement si besoin
// Declaration de la police
HFONT hFont, hFontTitre, hFontChamp, hFontBouton;

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

// Variables globales du Gestionnaires
LigneCSV lignes[MAX_LIGNES];
int nbLignes = 0;

// Pour gérer les combobox
HWND hCombo1;
HWND hCombo2;
char g_selectedUuid1[40] = "";
char g_selectedUuid2[40] = "";
wchar_t g_selectedText1[256] = L"";
wchar_t g_selectedText2[256] = L"";
// Fonctions de traitement
/*
void OpenAnnuaire(const wchar_t* folderPath, HWND hWnd) {
    // Vérifie si le dossier existe
    DWORD attribs = GetFileAttributesW(folderPath);
        if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Le dossier n'existe pas : %s", folderPath);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
        return;
    }
    wchar_t gestionnaire_path[MAX_PATH];
    swprintf(gestionnaire_path, MAX_PATH, L"%ls\\Annuaire.exe", (wchar_t*)folderPath);

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
            swprintf(msg, 128, L"Échec de l'ouverture de l'annuaire (code %ld).", (INT_PTR)result);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
    }
}

void OpenConfigurations(const wchar_t* folderPath, HWND hWnd) {
    // Vérifie si le dossier existe
    DWORD attribs = GetFileAttributesW(folderPath);
        if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Le dossier n'existe pas : %s", folderPath);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
        return;
    }
    wchar_t gestionnaire_path[MAX_PATH];
    swprintf(gestionnaire_path, MAX_PATH, L"%ls\\Configurations.exe", (wchar_t*)folderPath);

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
}*/

void LaunchSYD(const wchar_t* folderPath, wchar_t* sydPath, HWND hWnd) {
    // Vérifie si le dossier existe
    DWORD attribs = GetFileAttributesW(folderPath);
        if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Le dossier n'existe pas : %s", folderPath);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
        return;
    }
    wchar_t gestionnaire_path[MAX_PATH];
    swprintf(gestionnaire_path, MAX_PATH, L"%ls\\SyD.exe", (wchar_t*)folderPath);

    // Tente d'ouvrir avec ShellExecuteW
    HINSTANCE result = ShellExecuteW(
        NULL,                   // pas de fenêtre parente
        L"open",                // action
        gestionnaire_path,      // chemin du programme
        sydPath,                // paramètres
        NULL,                   // répertoire de travail
        SW_SHOWNORMAL           // mode d'affichage
    );

    // Vérifie le résultat
    if ((INT_PTR)result <= 32) {
            wchar_t msg[128];
            swprintf(msg, 128, L"Échec de l'ouverture du SyD (code %ld).", (INT_PTR)result);
            MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
    }
}

// Fonctions de l'interface du client 

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

// Remplissage de la combo box

void RemplirComboBox(HWND hComboBox) {
    SendMessageW(hComboBox, CB_RESETCONTENT, 0, 0);

    for (int i = 0; i < nbLignes; i++) {
        wchar_t texteW[256];
        // Conversion UTF-8 -> Unicode
        MultiByteToWideChar(CP_UTF8, 0, lignes[i].texte, -1, texteW, 256);

        // Ajout du texte
        int idx = (int)SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)texteW);

        // Associer l’index du tableau lignes[] à l’item
        SendMessageW(hComboBox, CB_SETITEMDATA, idx, (LPARAM)i);
    }
}

// Récupération et stockage dans variable globale
void StockerSelection1(HWND hComboBox1) {
    int index = (int)SendMessageW(hComboBox1, CB_GETCURSEL, 0, 0);
    if (index != CB_ERR) {
        int i = (int)SendMessageW(hComboBox1, CB_GETITEMDATA, index, 0);
        if (i >= 0) {
            // Copie du uuid2 dans la variable globale
            strcpy(g_selectedUuid1, lignes[i].uuid2);

            // Copie du texte affiché en Unicode
            SendMessageW(hComboBox1, CB_GETLBTEXT, index, (LPARAM)g_selectedText1);

            // Debug console
             printf("UUID selectionne : %s\n", g_selectedUuid1);
             wprintf(L"Texte affiche : %ls\n", g_selectedText1);
        }
    }
}

// Récupération et stockage dans variable globale
void StockerSelection2(HWND hComboBox2) {
    int index = (int)SendMessageW(hComboBox2, CB_GETCURSEL, 0, 0);
    if (index != CB_ERR) {
        int i = (int)SendMessageW(hComboBox2, CB_GETITEMDATA, index, 0);
        if (i >= 0) {
            // Copie du uuid2 dans la variable globale
            strcpy(g_selectedUuid2, lignes[i].uuid2);

            // Copie du texte affiché en Unicode
            SendMessageW(hComboBox2, CB_GETLBTEXT, index, (LPARAM)g_selectedText2);

            // Debug console
             printf("UUID selectionne : %s\n", g_selectedUuid2);
             wprintf(L"Texte affiche : %ls\n", g_selectedText2);
        }
    }
}

// Fontion de gestion des configurations

BOOL AssurerDossierConf() {
    DWORD attribs = GetFileAttributesW(L"conf");

    // Si le dossier existe ET est bien un répertoire
    if (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        return TRUE;
    }

    // Sinon on tente de le créer
    if (CreateDirectoryW(L"conf", NULL)) {
        return TRUE;
    } else {
        // Échec de création (permissions ?)
        return FALSE;
    }
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

BOOL SauvegarderConfig(const wchar_t *cheminFichier, const ConfigSVR *config) {
    FILE *f = _wfopen(cheminFichier, L"w, ccs=UTF-8"); // Ouverture en Unicode 
    if (!f) return FALSE;

    fprintf(f, "{\n");
    fprintf(f, "\"uuid\":\"%s\",\n", config->uuid);
    fprintf(f, "\"IP\":\"%s\",\n", config->ip);
    fprintf(f, "\"PORT\":\"%s\"\n", config->port);
    fprintf(f, "}\n");

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
/*
void ShowLicenceWindow(HWND hWnd) {
    HWND hLicenceWnd = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW, L"STATIC", L"A propos", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,hWnd, NULL, GetModuleHandle(NULL), NULL);

    HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY |WS_VSCROLL | WS_HSCROLL, 0, 0, 580, 360, hLicenceWnd, NULL, GetModuleHandle(NULL), NULL);

    // Exemple : charger depuis un fichier
    HANDLE hFile = CreateFileW(L"README.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD size = GetFileSize(hFile, NULL);
        char *buffer = malloc(size + 1);
        DWORD read;
        ReadFile(hFile, buffer, size, &read, NULL);
        buffer[read] = '\0';

        // Conversion en wide string
        int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
        wchar_t *wbuffer = malloc(wlen * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wbuffer, wlen);

        SetWindowTextW(hEdit, wbuffer);

        free(buffer);
        free(wbuffer);
        CloseHandle(hFile);
    }
}*/

//--------------------------------------------------------------------------------
// CallBack

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    
    static HWND hBtnSyD;
    static HWND hBtnRefresh;
    switch (msg) {

        // Zone de creation de l'interface graphique
        case WM_CREATE: {
            // Interface de "Menu"
            HMENU hMenubar = CreateMenu();
            HMENU hConfiguration = CreateMenu();
            //AppendMenuW(hConfiguration, MF_STRING, 12, L"Annuaire");
            AppendMenuW(hConfiguration, MF_STRING, 11, L"Initialiser");
            //AppendMenuW(hConfiguration, MF_STRING, BTN_REFRESH, L"Rafraichir");
            AppendMenuW(hConfiguration, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hConfiguration, MF_STRING, 13, L"Quitter");
            AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hConfiguration, L"Menu");
            // Blanc entre les menus
            //AppendMenu(hMenubar, MF_STRING, 0, L"                                                                                                              ");
            // Interface du Menu "?"
            //HMENU hAide = CreateMenu();
            //AppendMenuW(hAide, MF_STRING, 12, L"A propos");
            //AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hAide, L"?");
            
            SetMenu(hWnd, hMenubar);

            // Bandeau Entete
            // CreateWindowW(L"STATIC", L"Secure Your Data", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 20, 10, 370, 40, hWnd, NULL, NULL, NULL);

            // Zones de commande 
        
            CreateWindowW(L"STATIC", L"Ajouter des lecteurs (optionnel)", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 10, 10, 280, 20, hWnd, NULL, NULL, NULL);
            hCombo1 = CreateWindowW(L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL, 20, 40, 200, 200, hWnd, (HMENU)COMBOBOX1_ID, NULL, NULL);
            //CreateWindowW(L"BUTTON", L"R", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 310, 90, 20, 20, hWnd, (HMENU)BTN_REFRESH, NULL, NULL);     
            hCombo2 = CreateWindowW(L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL, 20, 70, 200, 200, hWnd, (HMENU)COMBOBOX2_ID, NULL, NULL);
            
            // Bouton refresh
            hBtnRefresh = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_ICON, 240, 50, 30, 30, hWnd, (HMENU)BTN_REFRESH, NULL, NULL);
            HICON hIconRefresh = (HICON)LoadImageW(NULL, L"img\\Refresh.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            if (hBtnRefresh) {
            // Associer l’icône au bouton
                SendMessageW(hBtnRefresh, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconRefresh);
            } else {
                MessageBoxW(hWnd, L"Impossible de charger l'icône", L"Erreur", MB_OK | MB_ICONERROR);
            }

            CreateWindowW(L"STATIC", L"Fabriquer", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 300, 10, 80, 20, hWnd, NULL, NULL, NULL);
            //CreateWindowW(L"BUTTON", L"Créer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 150, 170, 100, 40, hWnd, (HMENU)BTN_ASK, NULL, NULL);

            // Bouton ASK
            hBtnSyD = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_ICON, 315, 40, 50, 50, hWnd, (HMENU)BTN_ASK, NULL, NULL);
            HICON hIconSyD = (HICON)LoadImageW(NULL, L"img\\SyD.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            
            if (hIconSyD) {
                // Associer l’icône au bouton
                SendMessageW(hBtnSyD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconSyD);
            } else {
                MessageBoxW(hWnd, L"Impossible de charger l'icône", L"Erreur", MB_OK | MB_ICONERROR);
            }

            // Zone affichage de mon identifiant
            CreateWindowW(L"STATIC", L"Mon identifiant", WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 10, 110, 370, 20, hWnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER | SS_CENTERIMAGE, 40, 140, 330, 20, hWnd, (HMENU)401, NULL, NULL);  
            // Zone affichage bouton fermer
            //CreateWindowW(L"BUTTON", L"Ferme", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 310, 300, 0, 20, hWnd, (HMENU)BTN_CLOSE, NULL, NULL);

            ChargerCSV("annuaire.csv");
            RemplirComboBox(hCombo1);        
            RemplirComboBox(hCombo2); 
            ConfigSVR sydcfg;
            ConfigSVR spcfg;
            // Charger le fichier de config existant
            ChargerConfig(L"conf\\ServerSYD.cfg", &sydcfg);
            // Creation du chemin de lecture de mon UUID en fonction du SYD
            wchar_t syduuidW[40];
            MultiByteToWideChar(CP_UTF8, 0, sydcfg.uuid, -1, syduuidW, 40);
            swprintf(monUUID_PATH, MAX_PATH, L"conf\\%ls.CL", syduuidW);
            
            if ( ChargerMonUUID(monUUID_PATH, monUUID)) {
                SetWindowTextW(GetDlgItem(hWnd, 401), monUUID);
            }

        break;
        }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;
        if (GetDlgCtrlID(hCtrl) == BTN_ASK) {
            SetTextColor(hdc, RGB(139, 0, 0)); // rouge foncé
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }
        break;
    }

    case WM_DESTROY: 
    {
        DeleteObject(hFontTitre);
        DeleteObject(hFontChamp);
        DeleteObject(hFontBouton);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);

        // Zone d'affectation du comportement de l'interface graphique
        case WM_COMMAND: 
        {
            if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == COMBOBOX1_ID) {
                StockerSelection1(hCombo1);
            }
            if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == COMBOBOX2_ID) {
                StockerSelection2(hCombo2);
            }
            switch (LOWORD(wParam)) {

                // Bouton Demander
                case BTN_ASK: {
                    wchar_t g_selectedUuid1W[256] = L""; //uuid selectionné unicode
                    wchar_t g_selectedUuid2W[256] = L""; //uuid selectionné unicode
                    wchar_t uuidConcatW[1024] = L"";  // Concaténation Unicode
                    char uuidConcat[1024];
                    uuidConcatW[0] = L'\0';
                    uuidConcat[0] = '\0';
                    if (g_selectedUuid1[0] != '\0') {
                    // Convertir en wchar_t la valeur selectionnée dans la combobox
                        MultiByteToWideChar(CP_UTF8, 0, g_selectedUuid1, -1, g_selectedUuid1W, 256 );

                        // Concaténation dans uuidConcatW
                        wcscat(uuidConcatW, L"\"CL\": \"");
                        wcscat(uuidConcatW, g_selectedUuid1W);
                        wcscat(uuidConcatW, L"\"");
                        wcscat(uuidConcatW, L", ");
                    }


                    if (g_selectedUuid2[0] != '\0') {
                    // Convertir en wchar_t la valeur selectionnée dans la combobox
                        MultiByteToWideChar(CP_UTF8, 0, g_selectedUuid2, -1, g_selectedUuid2W, 256 );

                        // Concaténation dans uuidConcatW
                        wcscat(uuidConcatW, L"\"CL\": \"");
                        wcscat(uuidConcatW, g_selectedUuid2W);
                        wcscat(uuidConcatW, L"\"");
                        wcscat(uuidConcatW, L", ");
                    }

                    // ajout de monUUID pour faire la demande de TANK
                    wcscat(uuidConcatW, L"\"CL\": \"");
                    wcscat(uuidConcatW, monUUID);
                    wcscat(uuidConcatW, L"\"");
                    // Convertir en ANSI pour appel DLL
                    wprintf(L"uuidConcat : %ls\n", uuidConcatW);

                    WideCharToMultiByte(CP_ACP, 0, uuidConcatW, -1, uuidConcat, 1024, NULL, NULL);
                    // Convertir les chemins Unicode → ANSI
                    char confAnsi[MAX_PATH];
                    WideCharToMultiByte(CP_ACP, 0, CONF_PATH, -1, confAnsi, MAX_PATH, NULL, NULL);
                    char SYD_TANK[SIZE_BUFFER];
                    size_t SIZE_SYD_TANK = SIZE_BUFFER;
                    int ret_SYD_AskNewService =  SYD_AskNewService(confAnsi, uuidConcat, SYD_TANK, SIZE_SYD_TANK);

                    // Fonction de parsing des erreurs et affichage de l'erreur le cas echéant
 
                    // Affichage du retour
                    if (ret_SYD_AskNewService != 0) {
                        wchar_t msg[128];
                        swprintf(msg, 128, L"La demande de SYD a échoué avec le code %d", ret_SYD_AskNewService);
                        MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
                        break;
                    } 

                    // fonction d'enregistrement du SYD sur le disque (Unicode)
                    OPENFILENAMEW ofn;
                    WCHAR szFile[260] = { 0 };
                    HANDLE hFile;
                    DWORD dwBytesWritten;

                    ZeroMemory(&ofn, sizeof(ofn));

                    ofn.lStructSize = sizeof(ofn);
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = L"Fichiers SYD (*.syd)\0*.syd\0Fichiers texte (*.txt)\0*.txt\0Tous les fichiers (*.*)\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                    // Affiche la boîte de dialogue "Enregistrer sous"
                    if (GetSaveFileNameW(&ofn) == TRUE) {
                        // Crée ou écrase le fichier choisi
                        hFile = CreateFileW(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                        if (hFile != INVALID_HANDLE_VALUE) {
                            WriteFile(hFile, SYD_TANK, lstrlenA(SYD_TANK), &dwBytesWritten, NULL);
                            CloseHandle(hFile);
                            // MessageBoxW(NULL, L"Fichier enregistré avec succès !", L"Succès", MB_OK);
                            // Lancer l'ouverture du SyD créé.
                            LaunchSYD(DIRECTORY_PATH, ofn.lpstrFile, hWnd);
                        } else {
                            MessageBoxW(NULL, L"Erreur lors de la création du fichier.", L"Erreur", MB_ICONERROR);
                        }
                    } else {
                        MessageBoxW(NULL, L"Opération annulée.", L"Info", MB_OK);
                    }

                    break;
                }
                    
                case BTN_REFRESH: {
                    g_selectedUuid1[0] = '\0'; // je vide la chaine 
                    g_selectedUuid2[0] = '\0'; // je vide la chaine
                    SendMessageW(hCombo1, CB_RESETCONTENT, 0, 0); // je vide la combobox
                    SendMessageW(hCombo2, CB_RESETCONTENT, 0, 0); // je vide la combobox
                    ChargerCSV("annuaire.csv"); // je charge l'annuaire
                    RemplirComboBox(hCombo1);  // Je rempli la combo
                    RemplirComboBox(hCombo2);  // Je rempli la combo
                    break;
                }

                /*case 10: {
                    OpenConfigurations(DIRECTORY_PATH, hWnd);
                    break;
                }*/

                // Initialisation
                case 11: { // ID de l'entrée de menu "Initialisation"
                    // Convertir les chemins Unicode → ANSI
                    char confAnsi[MAX_PATH], bddAnsi[MAX_PATH];
                    WideCharToMultiByte(CP_ACP, 0, CONF_PATH, -1, confAnsi, MAX_PATH, NULL, NULL);
                    WideCharToMultiByte(CP_ACP, 0, BDD_PATH, -1, bddAnsi, MAX_PATH, NULL, NULL);

                    // Appel de la fonction
                    int ret = SYD_EnrollClient(confAnsi, bddAnsi);

                    // Affichage du retour
                    if (ret != 0) {
                        wchar_t msg[128];
                        swprintf(msg, 128, L"Enrolement Client a échoué avec le code %d", ret);
                        MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
                    } else {
                        // Appel de la fonction
                            int ret = SYD_SubscribeService(confAnsi, bddAnsi);

                            // Affichage du retour
                            if (ret == 201) {
                                MessageBoxW(hWnd, L"Initialisation réussie !", L"SYD", MB_OK);
                                if ( ChargerMonUUID(monUUID_PATH, monUUID)) {
                                    SetWindowTextW(GetDlgItem(hWnd, 401), monUUID);
                                }
                            } else if (ret == 200) {
                                MessageBoxW(hWnd, L"La souscription est déjà faite", L"Retour SYD", MB_OK);
                            }
                            else {
                                wchar_t msg[128];
                                swprintf(msg, 128, L"La souscription a échoué avec le code %d", ret);
                                MessageBoxW(hWnd, msg, L"Erreur", MB_ICONERROR);
                            }
                    }
                    break;
                }

                /*case 12: {
                    ShowLicenceWindow(hWnd);
                    break;
                }*/

                case 13: {// Quitter depuis le menu "Fichier"
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

// Creation des chemins des repertoires du client
    swprintf(BDD_PATH, MAX_PATH, L"%ls\\bdd\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(CONF_PATH, MAX_PATH, L"%ls\\conf\\", (wchar_t*)DIRECTORY_PATH);
    swprintf(TMP_PATH, MAX_PATH, L"%ls\\tmp\\", (wchar_t*)DIRECTORY_PATH);

    HWND hwnd = CreateWindowExW(0, L"FenetreBoutons", L"SyD",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, 420, 240,
                            NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Charger mon UUID en mémoire
    // Declaration des variables;
            ConfigSVR sydcfg;
            ConfigSVR spcfg;
            // Charger le fichier de config existant
            ChargerConfig(L"conf\\ServerSYD.cfg", &sydcfg);
            // Creation du chemin de lecture de mon UUID en fonction du SYD
            wchar_t syduuidW[40];
            MultiByteToWideChar(CP_UTF8, 0, sydcfg.uuid, -1, syduuidW, 40);
            swprintf(monUUID_PATH, MAX_PATH, L"conf\\%ls.CL", syduuidW);
            
            ChargerMonUUID(monUUID_PATH, monUUID);


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
