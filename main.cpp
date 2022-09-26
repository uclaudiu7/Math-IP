#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <fstream>

#define CLIP_ON 1
#define KEY_CENTER 76
#define MAX_STR 256
#define WindowWidth 1200
#define WindowHeight 800
#define colorcerc COLOR(48,124,160)
#define colorline WHITE

using namespace std;

string expresie;
int culoare = 1;
int caz = 1;
int okk = 0;
char numeimg[100]="";
char imginc[100]="";

struct nod{
    char info[MAX_STR];
    nod *stanga;
    nod *dreapta;
    int coloana;
    int xPrint, yPrint, lungime;
    int yBottom, yTop;
};
typedef nod * arbore;

arbore creazaNod(char info[]){
    nod* aux;
    aux = (nod*)malloc(sizeof(nod));
    if(aux == NULL)
        return (aux);
    aux->stanga = NULL;
    aux->dreapta = NULL;
    strcpy(aux->info, info);
    return aux;
}

int isGeometricFunction(string expresie, int i){
    vector < string > geo;
    geo.push_back("sin");
    geo.push_back("cos");
    geo.push_back("tg");
    geo.push_back("ctg");
    geo.push_back("arcsin");
    geo.push_back("arccos");
    geo.push_back("arctg");
    geo.push_back("arcctg");
    char c = expresie[i];
    int k = 0;
    string geometric = "";
    while(c >= 'a' && c <= 'z'){
        geometric += c;
        i++;
        k++;
        c = expresie[i];
    }
    for(int j = 0; j < geo.size(); ++j)
        if(geometric == geo[j])
            return k;
    return -1;
}

arbore construireArbore(arbore arb, string postfixare){
    int i = 0;
    stack < nod* > st;

    nod* arb_aux1;
    nod* arb_aux2;
    while(postfixare[i] != '\0'){
        int k = 1;
        if(postfixare[i] != ' '){
            if(isGeometricFunction(postfixare, i) != -1){
                k = isGeometricFunction(postfixare, i);
                string s = "";
                for(int j = i; j < i+k; ++j)
                    s += postfixare[j];
                char info[s.length()+1];
                strcpy(info, s.c_str());
                arb = creazaNod(info);

                arb_aux1 = st.top();
                st.pop();
                arb->dreapta = arb_aux1;
                st.push(arb);
            }
            else if(postfixare.substr(i, 4) == "sqrt"){
                string s = "sqrt";
                k = 4;
                char info[s.length()+1];
                strcpy(info, s.c_str());
                arb = creazaNod(info);

                arb_aux1 = st.top();
                st.pop();
                arb->stanga = arb_aux1;
                st.push(arb);
            }
            else if(!(postfixare[i] == '+' || postfixare[i] == '-' || postfixare[i] == '*'
                                      || postfixare[i] == '/' || postfixare[i] == '^' )){
                string s = "";
                while(postfixare[i] != ' '){
                    s += postfixare[i];
                    i++;
                }
                char info[s.length()+1];
                strcpy(info, s.c_str());
                arb = creazaNod(info);
                st.push(arb);
            }
            else{
                string s = "";
                s += postfixare[i];
                char info[s.length()+1];
                strcpy(info, s.c_str());
                arb = creazaNod(info);

                arb_aux1 = st.top();
                st.pop();
                arb_aux2 = st.top();
                st.pop();
                arb->dreapta = arb_aux1;
                arb->stanga = arb_aux2;
                st.push(arb);
            }
        }
        i += k;
    }
    return arb;
}

int ordineOperator(string c){
    if(c == "sin" || c == "cos" || c == "tg" || c == "ctg" || c == "sqrt")
        return 4;
    else if(c == "^")
        return 3;
    else if(c == "/" || c == "*")
        return 2;
    else if(c == "+" || c == "-")
        return 1;
    else
        return -1;
}

string postfixareExpresie(string expresie){
    /*
        ex:    x+y/2+(x-y)*3   ->   xy2/+xy-3*+
    */
    stack < string > st;
    string postfixare;

    int i = 0;
    char c = expresie[i];
    while(i < expresie.length()){
        // daca caracterul curent nu este operator il adaugam la rezultat
        string geometric = "";
        if(isGeometricFunction(expresie, i) != -1){
            int k = isGeometricFunction(expresie, i);

            string g = "";
            for(int j = i; j < i+k; ++j)
                g += expresie[j];

            while(!st.empty() && ordineOperator(g) <= ordineOperator(st.top())){
                postfixare += st.top();
                st.pop();
                postfixare += " ";
            }

            st.push(g);
            i += k;
            c = expresie[i];
        }
        else if(expresie.substr(i, 4) == "sqrt"){
            string g = "sqrt";
            while(!st.empty() && ordineOperator(g) <= ordineOperator(st.top())){
                postfixare += st.top();
                st.pop();
                postfixare += " ";
            }

            st.push(g);
            i += 4;
            c = expresie[i];
        }
        else if(c >= 'a' && c <= 'z'){
            while(c >= 'a' && c <= 'z'){
                postfixare += c;
                i++;
                c = expresie[i];
            }
            postfixare += " ";
        }
        else if(c >= '0' && c <= '9'){
            while((c >= '0' && c <= '9')){
                postfixare += c;
                i++;
                c = expresie[i];
            }
            postfixare += " ";
        }
        else if(c == '('){//daca este ( adaug la stiva
            st.push("(");
            i++;
            c = expresie[i];
        }
        else if(c == ')'){ //daca este ) dam pop la stiva pana intalnim ( si adaugam la rezultat
            while(st.top() != "("){
                postfixare += st.top();
                st.pop();
                postfixare += " ";
            }
            st.pop();
            i++;
            c = expresie[i];
        }
        else{   //cazul in care caracterul este operator
            string ch = "";
            ch += c;
            while(!st.empty() && ordineOperator(ch) <= ordineOperator(st.top())){
                postfixare += st.top();
                st.pop();
                postfixare += " ";
            }
            st.push(ch);
            i++;
            c = expresie[i];
        }
    }

    //dam pop la restul elementelor ramase in stiva
    while(!st.empty()){
        postfixare += st.top();
        st.pop();
        postfixare += " ";
    }
    cout << "postfixare:  " << postfixare << endl;
    return postfixare;
}

void afisareArbore(arbore arb){
    // subprogramul face o afisare in preordine (RSD)

    if (arb != NULL){
        for(int i = 0; i < strlen(arb->info); ++i)
            cout << arb->info[i];
        cout << " ";
		afisareArbore(arb->stanga);
		afisareArbore(arb->dreapta);
	}
}

bool esteOperator(string c){
    if(c.length() == 1 && strchr("+-*/^", c[0]))
        return true;
    return false;
}

int getYmax(arbore arb){
    if(arb == NULL)
        return 0;
    if(arb->stanga == NULL && arb->dreapta == NULL)
        return 1;
    if(strcmp(arb->info, "/") == 0)
        return 1 + getYmax(arb->stanga) + getYmax(arb->dreapta);
    if(strcmp(arb->info, "^") == 0)
        return getYmax(arb->stanga) + getYmax(arb->dreapta);
    else
        return max(getYmax(arb->stanga), getYmax(arb->dreapta));
}

int getXmax(arbore arb){
    if(arb != NULL){
        if(strcmp(arb->info, "/") == 0)
            return max(getXmax(arb->stanga), getXmax(arb->dreapta));
        if(strcmp(arb->info, "^") == 0)
            return getXmax(arb->stanga) + getXmax(arb->dreapta);
        return strlen(arb->info) + getXmax(arb->stanga) + getXmax(arb->dreapta);
    }
    else
        return 0;
}

int getYstart(arbore arb){
    if(arb != NULL){
        if(strcmp(arb->info, "/") == 0)
            return 1 + getYmax(arb->stanga);
        else if(strcmp(arb->info, "^") == 0)
            return 1 + getYmax(arb->stanga) + getYmax(arb->dreapta);
        else if(strcmp(arb->info, "+") == 0 || strcmp(arb->info, "-") == 0 || strcmp(arb->info, "*") == 0)
            return max(getYstart(arb->stanga), getYstart(arb->dreapta));
        else
            return 1;
    }
}

int getDownHeight(arbore arb){
    if(arb != NULL){
        if(strcmp(arb->info, "/") == 0)
            return 1 + getYmax(arb->dreapta);
        else if(strcmp(arb->info, "^") == 0)
            return 1;
        else if(strcmp(arb->info, "+") == 0 || strcmp(arb->info, "-") == 0 || strcmp(arb->info, "*") == 0)
            return max(getDownHeight(arb->stanga), getDownHeight(arb->dreapta));
        else
            return 1;
    }
}

int getUpHeight(arbore arb){
    if(arb != NULL){
        if(strcmp(arb->info, "/") == 0)
            return 1 + getYmax(arb->stanga);
        else if(strcmp(arb->info, "^") == 0)
            return getYmax(arb->stanga) + getYmax(arb->dreapta);
        else if(strcmp(arb->info, "+") == 0 || strcmp(arb->info, "-") == 0 || strcmp(arb->info, "*") == 0)
            return max(getUpHeight(arb->stanga), getUpHeight(arb->dreapta));
        else
            return 1;
    }
}

void determinaCoordonate(arbore arb, int& x, int& y){
    if(arb != NULL){
        if(strcmp(arb->info, "/") == 0){
            /*partea de sus a fractiei*/
            arb->xPrint = x;
            arb->yPrint = y;
            int hMax = getDownHeight(arb->stanga);
            y -= hMax;
            determinaCoordonate(arb->stanga, x, y);
            /*^^^^^^^^^^^^^^^^^^^^^^^^*/

            /*!!!!*/

            /*partea de jos a fractiei*/
            hMax = getYmax(arb->dreapta);
            int lMax = getXmax(arb);
            int yStart = getYstart(arb->dreapta);
            x = arb->xPrint;
            y = arb->yPrint;
            y = y + yStart;
            arb->lungime = lMax;
            determinaCoordonate(arb->dreapta, x, y);
            /*^^^^^^^^^^^^^^^^^^^^^^^^*/
            x = arb->xPrint + lMax;
            y = arb->yPrint;
        }
        else if(strcmp(arb->info, "^") == 0){
            arb->xPrint = x;
            arb->yPrint = y;
            determinaCoordonate(arb->stanga, x, y);

            y = arb->yPrint;
            y -= getUpHeight(arb->stanga);
            y -= getDownHeight(arb->dreapta);
            y++;

            determinaCoordonate(arb->dreapta, x, y);
        }
        else if(strcmp(arb->info, "sqrt") == 0){
            arb->lungime = getXmax(arb->stanga);
            arb->xPrint = x;
            arb->yPrint = y;
            determinaCoordonate(arb->stanga, x, y);
        }
        else{
            determinaCoordonate(arb->stanga, x, y);
            arb->xPrint = x;
            arb->yPrint = y;
            x += strlen(arb->info);
            determinaCoordonate(arb->dreapta, x, y);
        }
    }
}

void printCoordonate(arbore arb){
    if(arb != NULL){
        printCoordonate(arb->stanga);
        cout << "pentru ";
        for(int i = 0; i < strlen(arb->info); ++i)
            cout << arb->info[i];
        cout << "   x:   " << arb->xPrint << "    y:   " << arb->yPrint << endl;
        printCoordonate(arb->dreapta);
    }
}

void printSqrt(int x, int sus, int jos, int l){
    setlinestyle(0,1,1);
    line(x, jos, x-5, sus);
    line(x, jos, x-10, jos-10);
    line(x-5, sus, x-5+l, sus);
}

void scrie(arbore arb){
    string geom = "sin cos tg ctg arcsin arccos arctg arcctg";

    if(arb != NULL){
        int gap = 30;
        int x = gap * arb->xPrint;
        int y = gap * arb->yPrint;

        scrie(arb->stanga);

        if(strcmp(arb->info, "sqrt") == 0){
            int sus = y - gap*(getUpHeight(arb->stanga)-1);
            int jos = y + gap*getDownHeight(arb->stanga);
            int l = gap*getXmax(arb->stanga);
            printSqrt(x-5, sus-5, jos, l);
        }
        else if(strcmp(arb->info, "/") == 0){
            setlinestyle(0,1,2);
            line(x, y+20, x + 25*arb->lungime, y+20);
        }
        else if(strcmp(arb->info, "^") == 0){
            //nimic
        }
        else if(strcmp(arb->info, "*") == 0){
            circle(x+10, y+20, 2);
        }
        else
            outtextxy(x, y, arb->info);

        scrie(arb->dreapta);
    }
}

int k=0;
int xc1=0,yc1=0;
int winw=WindowWidth;
int winh=WindowHeight;

void inordine(arbore a){
    if(a!=NULL){
        inordine(a->stanga);
        a->coloana=++k; //dupa o parcurgere inordine, fiecare nod va avea in structura sa (pe campul 'coloana') coloana pe care se afla
        inordine(a->dreapta);
    }
}

void linie(int x1,int y1,int x2,int y2){
    int d=10,h=10; // d lungimea sagetii, h inaltimea sagetii;
    line(x1,y1,x2,y2);
    int dx = x2 - x1, dy = y2 - y1;
    double D =sqrt(dx*dx + dy*dy); //distanta liniei
    double xm = D - d, xn = xm, ym = h, yn = -h, x; //initializarea coordonatelor sagetii in functie de lungimea si inaltimea data anterior
    double sin = dy / D, cos = dx / D;

    x = xm*cos - ym*sin + x1;
    ym = xm*sin + ym*cos + y1; //coordonata y din care pleaca primul capat al sagetii;
    xm = x; //coordonata x din care pleaca primul capat al sagetii;

    x = xn*cos - yn*sin + x1;
    yn = xn*sin + yn*cos + y1; //coordonaya y din care pleaca al doilea capat al sagetii;
    xn = x; // coordonata x din care pleaca al doilea capat al sagetii;
    line(xm,ym,x2,y2);
    line(xn,yn,x2,y2);
}

int nrNiveluri(arbore a){
    if(a==NULL)
        return 0;
    else{
        int n1 = nrNiveluri(a->stanga);
        int n2 = nrNiveluri(a->dreapta);
        return 1+max(n1,n2);
    }
}

int nrColoane(arbore a){
    if(a==NULL)
        return 0;
    else{
        int n1=nrColoane(a->stanga);
        int n2=nrColoane(a->dreapta);
        return 1+n1+n2;
    }
}

void centrucasuta(int n,int m,int i,int j,int &xc,int &yc){
    // winw/winh=window width/height; n si m sunt indicii numarului maxim de casute; i si j indicii casutei careia i se cauta centrul;
    int ki,kj;
    ki=winw/m;
    kj=winh/n;
    xc=(j*ki)-(ki/2);
    yc=(i*kj)-(kj/2);
    // prin xc si yc se va stabili centrul casutei corespondente liniei i si coloanei j
}

void deseneaza(int nmax,int mmax,char n[],int linie,int coloana,int &xc,int &yc){
    centrucasuta(nmax,mmax,linie,coloana,xc,yc); //xc si yc vor avea dupa apel valoarea in care se deseneaza nodul
    setcolor(colorcerc);
    setlinestyle(0,1,4);
    circle(xc,yc,30);
    setcolor(colorline);
    settextstyle(8,0,1);
    outtextxy(xc-8,yc-8,n);
}

void desenarb(int nmax,int mmax,arbore a,int niv){
    int xc,yc;
    if(a!=NULL){
        deseneaza(nmax,mmax,a->info,niv,a->coloana,xc,yc);
        desenarb(nmax,mmax,a->stanga,niv+1);
        if((xc1*yc1!=0)&&(a->stanga!=NULL)) linie(xc-22,yc+22,xc1+22,yc1-22); //linia(sageata) pentru fiul stanga
            desenarb(nmax,mmax,a->dreapta,niv+1);
        if((xc1*yc1!=0)&&(a->dreapta!=NULL)) linie(xc+22,yc+22,xc1-22,yc1-22); //linia(sageata) pentru fiul dreapta
            deseneaza(nmax,mmax,a->info,niv,a->coloana,xc1,yc1);
    }
}

int okg = 1;

void verif(char s[])
{ int i;
    int frpd=0,frpi=0,ok = 1;
    int x=50,y=50;
    for(i=0; i<strlen(s); i++)
        {if(s[i]=='/' && s[i+1]=='0')
            {outtextxy(x,y,"Impartire la 0");
            ok = 0;
            y=y+30;
            }
        if (s[i]=='(')
            frpd++;
        if(s[i]==')')
             {  frpi++;
                  if(frpi>frpd)
                    {outtextxy(x,y,"Inchidere fara deschidere anterioara");
                        y=y+30;
                     ok = 0;
                    }
              }
        if (strchr("+-*/^",s[i])!=NULL && strchr("+-*/^",s[i+1])!=NULL)
              {  char error[100]="";
              error[0]=s[i];
              strcat(error," inainte de ");
              error[strlen(error)] = s[i+1];

                outtextxy(x,y,error);
                y=y+30;
                ok = 0;
              }
      /*
        if(isalpha(s[i]) && isalnum(s[i+1]))
        {  outtextxy(x,y,"Variabila introdusa gresit");
        y=y+30;
            ok = 0;
        }
      */
        if(isalnum(s[i]) && isalpha(s[i]) == 0 && isalpha(s[i+1]) || (isalnum(s[i]) && isalpha(s[i]) == 0 && isalpha(s[i-1]) && strchr("gnst",s[i-1])==NULL))
        {   outtextxy(x,y,"Un numar introdus contine litere");
            ok = 0;
            y=y+30;
        }
        }
     if(frpd!=frpi)
            {outtextxy(x,y,"Deschidere fara inchidere");
            ok = 0;
            y=y+30;
            }
     if(ok == 0)
        okg = 0;
        else
            outtextxy(x,y,"EXPRESIE OK");
}

void CitesteSir(int x, int y, char mesaj[MAX_STR], char S[MAX_STR] ){
    char Enter = 13;
    char BackSpace = 8;
    char Escape = 27;
    char s2[2];
    s2[1]='\0';
    char MultimeDeCaractereAcceptabile[MAX_STR]="0123456789abcdefghijklmnopqrstuvwxyz+-^/()_*=.<> ";
    char S_initial[MAX_STR];
    char tasta;
    char S_[MAX_STR];
    char mesaj_[MAX_STR];

    strcpy(mesaj_,mesaj);
    strcpy(S_initial,S);
    settextstyle(8,0,2);
    if(culoare % 2 == 1)
        setcolor(COLOR(255,255,255));
    else
        setcolor(COLOR(26,26,26));
    settextjustify(0,0);
    outtextxy(x,y,mesaj_);

    x=x+textwidth(mesaj);
    strcpy(S,"");
    strcpy(S_,S);
    strcat(S_,"_");
    setcolor(BLACK);
    outtextxy(x,y,S_);
    s2[0]=tasta;
    strcat(S,s2);
    strcpy(S_,S);
    strcat(S_,"_");
    if(culoare % 2 == 1)
        setcolor(COLOR(255,255,255));
    else
        setcolor(COLOR(26,26,26));
    outtextxy(x,y,S_);

    do{
        tasta=getch();
        if (tasta==0){
            tasta=getch();
            Beep(1000,500);
        }

        else
            if (strchr(MultimeDeCaractereAcceptabile, tasta)){
                // stergem fostul sir
                strcpy(S_,S);
                strcat(S_,"_");
                if(culoare % 2 == 1)
                    setcolor(COLOR(255,255,255));
                else
                    setcolor(COLOR(26,26,26));
                outtextxy(x,y,S_);
                s2[0]=tasta;
                strcat(S,s2);
                strcpy(S_,S);
                strcat(S_,"_");
                if(culoare % 2 == 1)
                    setcolor(COLOR(255,255,255));
                else
                    setcolor(COLOR(26,26,26));
                outtextxy(x,y,S_);
            }
        if (tasta==BackSpace)
            if (!(strcmp(S,"")))
                Beep(500,100);
            else{
                if(culoare % 2 == 1)
                    setcolor(COLOR(26,26,26));
                else
                    setcolor(COLOR(246,239,239));
                outtextxy(x,y,S_);
                if(culoare % 2 == 1)
                    setcolor(COLOR(255,255,255));
                else
                    setcolor(COLOR(26,26,26));
                S[strlen(S)-1]='\0';
                strcpy(S_,S);
                strcat(S_,"_") ;
                outtextxy(x,y,S_);
                Beep(200,20);
            }
        else if (tasta!=Enter && tasta!=Escape){
            Beep(200,20);
        }
    }
    while (tasta!=Enter && tasta!=Escape);
    if (tasta == Escape)
        strcpy(S,S_initial);

    if(culoare % 2 == 1)
        setcolor(COLOR(26,26,26));
    else
        setcolor(COLOR(246,239,239));
    outtextxy(x,y,S_);

    if(culoare % 2 == 1)
        setcolor(COLOR(255,255,255));
    else
        setcolor(COLOR(1,17,10));
    outtextxy(x,y,S);

    if(caz == 1)
     expresie = S;
      else if(caz == 2)
        strcpy(numeimg,S);
            else
                strcpy(imginc,S);
}

void numeimagine();
void afisareGrafica(arbore arb);
void setari();
void introducere();
void meniu();
void instructiuni();
void afisarb();
void incarcaimagine();
void eval();

void testbutoanemeniu(){
    bool ok=true;
    while(ok){
        if(mousex()>120 && mousex()<380 && mousey()>90 && mousey()<140 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            introducere();
        }
        if(mousex()>120 && mousex()<380 && mousey()>170 && mousey()<220 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
             incarcaimagine();
        }
        if(mousex()>120 && mousex()<380 && mousey()>250 && mousey()<300 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);

            string postfixare = postfixareExpresie(expresie);
            if(okg == 1)
            {arbore arb = NULL;
            arb = construireArbore(arb, postfixare);
            afisareGrafica(arb);
            }
            else
                eval();
        }
        if(mousex()>120 && mousex()<380 && mousey()>330 && mousey()<380 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            afisarb();
        }
        if(mousex()>120 && mousex()<380 && mousey()>410 && mousey()<460 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            instructiuni();
        }
        if(mousex()>120 && mousex()<380 && mousey()>490 && mousey()<540 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            setari();
        }
        if(mousex()>390 && mousex()<420 && mousey()>270 && mousey()<300 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            eval();
        }

    }
}

void testbutoanesetari(){
    bool ok=true;
    while(ok){
        if(mousex()>120 && mousex()<380 && mousey()>90 && mousey()<140 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            culoare++;
            setari();
        }
        if(mousex()>120 && mousex()<380 && mousey()>170 && mousey()<220 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            //ceva la muzica
        }
        if(mousex()>390 && mousex()<480 && mousey()>645 && mousey()<685 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            meniu();
        }
    }
}

void eval()
{   closegraph();
    initwindow(900,700,"EVALUATOR",200,200);
    setbkcolor(COLOR(26,26,26));
    cleardevice();
    int n = expresie.length();
        char s[n+1];
        strcpy(s, expresie.c_str());
    settextstyle(8,0,2);
    setcolor(COLOR(255,255,255));
       verif(s);
    setcolor(COLOR(48,124,160));
    setlinestyle(0,1,4);
    rectangle(780,640,880,680);
    setcolor(COLOR(255,255,255));
    outtextxy(790,650,"INAPOI");
    bool ok=true;
    while(ok)
        if(mousex()>780 && mousex()<880 && mousey()>640 && mousey()<680 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
                meniu();
          }
}

void incarcaimagine(){
    closegraph();
    initwindow(900,700,"ImagineIncarcata",200,200);
    setbkcolor(COLOR(26,26,26));
    cleardevice();

    char expresia[MAX_STR]="";
    char mesaj[MAX_STR];
    strcpy(mesaj,"NUMELE IMAGINII CAUTATE: ");

    caz = 3;

    CitesteSir(10,30,mesaj,expresia);

    closegraph();
    strcat(imginc,".jpg");
    initwindow(1400,700,"ImagineIncarcata",200,200);
    setbkcolor(COLOR(26,26,26));
    cleardevice();
    readimagefile(imginc,0,0,1400,500);
    setcolor(COLOR(48,124,160));
    setlinestyle(0,1,4);
    rectangle(1280,640,1380,680);
    settextstyle(8,0,2);
    setcolor(COLOR(255,255,255));
    outtextxy(1295,650,"INAPOI");

    bool ok=true;
    while(ok)
        if(mousex()>1280 && mousex()<1380 && mousey()>640 && mousey()<680 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
                meniu();
          }
}

void setari(){
    closegraph();
    initwindow(500,700,"SETARI",600,200);

    if(culoare % 2 == 1){
        setbkcolor(COLOR(26,26,26));
        cleardevice();

        setcolor(COLOR(48,124,160));
        setlinestyle(0,1,4);
        rectangle(120,90,380,140);
        rectangle(120,170,380,220);
        rectangle(390,645,480,685);

        setcolor(COLOR(255,255,255));
    }
    else{
        setbkcolor(COLOR(246,239,239));
        cleardevice();

        setcolor(COLOR(10,67,40));
        setlinestyle(0,1,4);
        rectangle(120,90,380,140);
        rectangle(120,170,380,220);
        rectangle(390,645,480,685);

        setcolor(COLOR(1,17,10));
    }
    settextstyle(8,0,4);
    outtextxy(175,10,"SETARI");

    settextstyle(8,0,2);
    outtextxy(175,105,"SCHIMBA TEMA");
    outtextxy(145,185,"START/STOP MUZICA");
    outtextxy(400,655,"INAPOI");

    testbutoanesetari();
}

void numeimagine(){
    initwindow(900,700,"DenumireImagine",200,200);

        setbkcolor(COLOR(26,26,26));
        cleardevice();

    char expresia[MAX_STR]="";
    char mesaj[MAX_STR];
    strcpy(mesaj,"NUMELE IMAGINII: ");

    caz = 2;

    CitesteSir(10,30,mesaj,expresia);

    closegraph();
}

void afisareGrafica(arbore arb){
    closegraph();
    initwindow(1400,700,"Expresia Grafica",200,200);

    if(culoare % 2 == 1){
        setbkcolor(COLOR(26,26,26));
        cleardevice();

        setcolor(COLOR(48,124,160));
        setlinestyle(0,1,4);
        rectangle(1280,640,1380,680);
        rectangle(1130,640,1250,680);

        setcolor(COLOR(255,255,255));
    }
    else{
        setbkcolor(COLOR(246,239,239));
        cleardevice();

        setcolor(COLOR(10,67,40));
        setlinestyle(0,1,4);
        rectangle(1280,640,1380,680);
        rectangle(1130,640,1250,680);
        setcolor(COLOR(1,17,10));
    }

    settextstyle(8,0,2);
    outtextxy(1295,650,"INAPOI");
    outtextxy(1140,650,"SALVEAZA");

    int x = 1;
    int y = getYstart(arb)+2;

    determinaCoordonate(arb, x, y);
    printCoordonate(arb);

    //cout << "y de start este:  " << getYstart(arb) << endl;
    settextstyle(8,0,5);
    scrie(arb);

        if(okk == 1)
             {writeimagefile(numeimg,1,1,1400,500);
                okk = 0;
             }
    bool ok=true;
    while(ok)
        {if(mousex()>1280 && mousex()<1380 && mousey()>640 && mousey()<680 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            meniu();
          }
          if(mousex()>1130 && mousex()<1250 && mousey()>640 && mousey()<680 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
                    numeimagine();
                    strcat(numeimg,".jpg");
                        okk=1;
                    afisareGrafica(arb);

                   // meniu();
          }

        }
}

void instructiuni(){
    closegraph();
    initwindow(900,700,"INSTRUCTIUNI");

    if(culoare % 2 == 1){
        setbkcolor(COLOR(26,26,26));
        cleardevice();

        setcolor(COLOR(48,124,160));
        setlinestyle(0,1,4);
        rectangle(780,640,870,680);

        setcolor(COLOR(255,255,255));
    }
    else{
        setbkcolor(COLOR(246,239,239));
        cleardevice();

        setcolor(COLOR(10,67,40));
        setlinestyle(0,1,4);
        rectangle(780,640,870,680);

        setcolor(COLOR(1,17,10));
    }

    settextstyle(8,0,4);
    outtextxy(70,10,"INSTRUCTIUNI DE INTRODUCERE A FORMULEI");
    settextstyle(8,0,2);
    outtextxy(790,650,"INAPOI");
    readimagefile("Reguli.jpg",50,50,850,650);
    bool ok=true;
    while(ok)
        if(mousex()>780 && mousex()<870 && mousey()>640 && mousey()<680 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            meniu();
        }
}

void afisarb(){
    closegraph();
    int nmax,mmax;;
    arbore a = NULL;
    string postfixare = postfixareExpresie(expresie);
    a = construireArbore(a, postfixare);
    nmax=nrNiveluri(a);
    mmax=nrColoane(a);
    // cout<<endl<<"   "<<nmax<<"  "<<mmax;
    initwindow(WindowWidth,WindowHeight,"AfisareArbore");

    setbkcolor(COLOR(26,26,26));
    cleardevice();

    inordine(a);
    desenarb(nmax,mmax,a,1);

    setcolor(COLOR(48,124,160));
    setlinestyle(0,1,4);
    rectangle(1080,20,1180,60);

    setcolor(COLOR(255,255,255));
    settextstyle(8,0,2);
    outtextxy(1090,30,"INAPOI");

    bool ok = true;
     while(ok)
         if(mousex()>1080 && mousex()<1180 && mousey()>20 && mousey()<60 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
              meniu();
            }
}

void introducere(){
    closegraph();
    initwindow(1600,400,"IntroducereExpresie",200,200);
    if( culoare % 2 == 1){
        setbkcolor(COLOR(26,26,26));
        cleardevice();
    }
    else{
        setbkcolor(COLOR(246,239,239));
        cleardevice();
    }

    char expresia[MAX_STR]="";
    char mesaj[MAX_STR];
    strcpy(mesaj,"INTRODUCETI EXPRESIA: ");
    caz = 1;
    CitesteSir(10,30,mesaj,expresia);
    if(culoare % 2 == 1){
        setcolor(COLOR(48,124,160));
        setlinestyle(0,1,4);
        rectangle(1480,340,1570,380);

        setcolor(COLOR(255,255,255));
    }
    else{
        setcolor(COLOR(10,67,40));
        setlinestyle(0,1,4);
        rectangle(1480,340,1570,380);

        setcolor(COLOR(1,17,10));
    }
    settextstyle(8,0,2);
    outtextxy(1490,370,"INAPOI");

    bool ok=true;
    while(ok)
        if(mousex()>1480 && mousex()<1570 && mousey()>340 && mousey()<380 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            cout << "expresia introdusa:  " << expresie << endl;
            string postfixare = postfixareExpresie(expresie);
            cout << "postfixare:  " << postfixare << endl;
            eval();
            if(okg == 1)
            {arbore arb = NULL;
            arb = construireArbore(arb, postfixare);
            cout << "afisarea RSD a arborelui:  ";
            afisareArbore(arb);
            }
            meniu();
        }
}

void meniu(){
    closegraph();
    initwindow(500,700,"MENIU",600,200);
    if(culoare % 2 == 1){
        setbkcolor(COLOR(26,26,26));
        cleardevice();

        setcolor(COLOR(48,124,160));
        setlinestyle(0,1,4);
        rectangle(120,90,380,140);
        rectangle(120,170,380,220);
        rectangle(120,250,380,300);
        rectangle(120,330,380,380);
        rectangle(120,410,380,460);
        rectangle(120,490,380,540);
        rectangle(30,600,470,680);

        rectangle(390,270,420,300);

        setcolor(COLOR(255,255,255));
    }
    else{
        setbkcolor(COLOR(246,239,239));
        cleardevice();

        setcolor(COLOR(10,67,40));
        setlinestyle(0,1,4);
        rectangle(120,90,380,140);
        rectangle(120,170,380,220);
        rectangle(120,250,380,300);
        rectangle(120,330,380,380);
        rectangle(120,410,380,460);
        rectangle(120,490,380,540);
        rectangle(30,600,470,680);
        rectangle(390,270,420,300);

        setcolor(COLOR(1,17,10));
    }

    settextstyle(8,0,4);
    outtextxy(105,10,"MENIU PRINCIPAL");

    settextstyle(8,0,2);
    outtextxy(398,275,"V");
    outtextxy(130,105,"INTRODUCERE EXPRESIE");
    outtextxy(155,185,"INCARCA EXPRESIE");
    outtextxy(155,265,"AFISARE EXPRESIE");
    outtextxy(165,345,"AFISARE ARBORE");
    outtextxy(180,425,"INSTRUCTIUNI");
    outtextxy(215,505,"SETARI");

    settextstyle(8,0,1);

    if(expresie.empty())
        outtextxy(37,630,"INTRODUCETI O EXPRESIE -> PRIMUL BUTON");
    else{
        int n = expresie.length();
        char ptafis[n+1];
        strcpy(ptafis, expresie.c_str());
        outtextxy(40,630,ptafis);
    }
    testbutoanemeniu();
}

int main(){
    meniu();
    getch();
    closegraph();
    return 0;
}
