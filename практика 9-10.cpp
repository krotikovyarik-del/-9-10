#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum Type { CHARTER, TRANSIT, CONNECTION };

struct Time {
    int h = 0;
    int m = 0;
};

struct Flight {
    string dest = "";
    Time dep;
    Time arr;
    int dur = 0;
    double cost = 0;
    Type type = TRANSIT;
};

struct BinFlight {
    char dest[50]{};
    Time dep;
    Time arr;
    int dur = 0;
    double cost = 0;
    int type = 0;
};

string typeStr(Type t) {
    if (t == CHARTER) return "Чартер";
    if (t == TRANSIT) return "Транзит";
    return "Стыковка";
}

void showTime(Time t) {
    if (t.h < 10) cout << '0';
    cout << t.h << ':';
    if (t.m < 10) cout << '0';
    cout << t.m;
}

void show(const Flight& f) {
    cout << "Пункт: " << f.dest << "\nВылет: ";
    showTime(f.dep);
    cout << "\nПрилет: ";
    showTime(f.arr);
    cout << "\nДлит: " << f.dur
        << "\nЦена: " << f.cost
        << "\nТип: " << typeStr(f.type)
        << "\n--------------------\n";
}

void showAll(const Flight a[], int n, string title) {
    cout << "\n=== " << title << " ===\n";
    if (n == 0) {
        cout << "Нет данных\n";
        return;
    }
    for (int i = 0; i < n; i++) show(a[i]);
}

bool isInsk(string s) {
    return s.size() >= 4 && s.substr(s.size() - 4) == "инск";
}

int filtInsk(const Flight a[], int n, Flight b[]) {
    int k = 0;
    for (int i = 0; i < n; i++)
        if (isInsk(a[i].dest) && a[i].type == CONNECTION)
            b[k++] = a[i];
    return k;
}

void sortDur(Flight a[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (a[j].dur > a[j + 1].dur) {
                Flight t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
}

int findFlight(const Flight a[], int n, string d) {
    for (int i = 0; i < n; i++)
        if (a[i].dest == d) return i;
    return -1;
}

void edit(Flight& f, string d, Time dep, Time arr, int dur, double cost, Type t) {
    f.dest = d;
    f.dep = dep;
    f.arr = arr;
    f.dur = dur;
    f.cost = cost;
    f.type = t;
}

void show2(const Flight a[], int n) {
    int k = 0;
    cout << "\n=== 2 утренних чартерных ===\n";
    for (int i = 0; i < n && k < 2; i++) {
        if (a[i].type == CHARTER && a[i].dep.h >= 6 && a[i].dep.h < 12) {
            show(a[i]);
            k++;
        }
    }
}

int filtCost(const Flight a[], int n, Flight b[], double x) {
    int k = 0;
    for (int i = 0; i < n; i++)
        if (a[i].type == CHARTER && a[i].cost > x)
            b[k++] = a[i];
    return k;
}

void saveTxt(const Flight a[], int n, string file) {
    ofstream fout(file);
    for (int i = 0; i < n; i++) {
        fout << a[i].dest << " "
            << a[i].dep.h << " " << a[i].dep.m << " "
            << a[i].arr.h << " " << a[i].arr.m << " "
            << a[i].dur << " " << a[i].cost << " " << a[i].type << "\n";
    }
    fout.close();
}

void readCost(Flight a[], int n, string file) {
    ifstream fin(file);
    if (!fin.is_open()) {
        cout << "\nФайл prices.txt не найден\n";
        return;
    }

    string d;
    double c;

    while (fin >> d >> c) {
        for (int i = 0; i < n; i++) {
            if (a[i].dest == d) a[i].cost = c;
        }
    }

    fin.close();
}

void toBin(const Flight& f, BinFlight& b) {
    int i = 0;
    for (; i < (int)f.dest.size() && i < 49; i++) b.dest[i] = f.dest[i];
    b.dest[i] = '\0';
    b.dep = f.dep;
    b.arr = f.arr;
    b.dur = f.dur;
    b.cost = f.cost;
    b.type = f.type;
}

void fromBin(const BinFlight& b, Flight& f) {
    f.dest = b.dest;
    f.dep = b.dep;
    f.arr = b.arr;
    f.dur = b.dur;
    f.cost = b.cost;
    f.type = (Type)b.type;
}

void saveBin(const Flight a[], int n, string file) {
    ofstream fout(file, ios::binary);
    if (!fout.is_open()) return;

    fout.write((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        BinFlight b{};
        toBin(a[i], b);
        fout.write((char*)&b, sizeof(b));
    }
    fout.close();
}

int readBin(Flight a[], string file) {
    ifstream fin(file, ios::binary);
    if (!fin.is_open()) return 0;

    int n = 0;
    fin.read((char*)&n, sizeof(n));

    for (int i = 0; i < n; i++) {
        BinFlight b{};
        fin.read((char*)&b, sizeof(b));
        fromBin(b, a[i]);
    }

    fin.close();
    return n;
}

int main() {
    setlocale(LC_ALL, "");

    const int N = 20;

    Flight a[N] = {
        {"Минск", {6,30}, {8,10}, 100, 4500, CHARTER},
        {"Новосибирск", {9,15}, {13,45}, 270, 8200, CONNECTION},
        {"Пинск", {14,20}, {15,40}, 80, 3000, TRANSIT},
        {"Смоленск", {7,50}, {9,30}, 100, 5200, CHARTER},
        {"Брест", {12,10}, {13,20}, 70, 2800, TRANSIT},
        {"Каменск", {16,40}, {19,10}, 150, 6100, CONNECTION},
        {"Томск", {5,55}, {9,25}, 210, 9500, CHARTER},
        {"Мурманск", {11,0}, {14,20}, 200, 7300, TRANSIT},
        {"Дзержинск", {8,25}, {10,55}, 150, 6800, CONNECTION},
        {"Омск", {13,35}, {17,5}, 210, 7900, CHARTER},
        {"Челябинск", {10,10}, {14,40}, 270, 8800, CONNECTION},
        {"Гданьск", {6,45}, {8,50}, 125, 4700, CHARTER},
        {"Красноярск", {15,25}, {20,5}, 280, 9900, TRANSIT},
        {"Витебск", {7,5}, {8,15}, 70, 2600, CHARTER},
        {"Минск", {18,0}, {19,35}, 95, 4100, CONNECTION},
        {"Севастополь", {9,50}, {12,30}, 160, 6400, TRANSIT},
        {"Курганинск", {4,40}, {8,20}, 220, 9100, CONNECTION},
        {"Норильск", {11,30}, {16,45}, 315, 12000, CHARTER},
        {"Волжск", {8,0}, {9,25}, 85, 3900, TRANSIT},
        {"Иркутск", {6,10}, {11,50}, 340, 13500, CHARTER}
    };

    showAll(a, N, "Все рейсы");

    Flight b[N]{};
    int m = filtInsk(a, N, b);
    showAll(b, m, "Инск + стыковка");

    sortDur(b, m);
    showAll(b, m, "Сортировка");

    int id = findFlight(a, N, "Новосибирск");
    if (id != -1) {
        cout << "\n=== Конкретный рейс ====\n";
        show(a[id]);
    }

    show2(a, N);

    int u = findFlight(a, N, "Томск");
    if (u != -1) {
        edit(a[u], "Хабаровск", { 10,20 }, { 16,10 }, 350, 15000, CONNECTION);
        cout << "\n=== После изменения ===\n";
        show(a[u]);
    }

    Flight c[N]{};
    int k = filtCost(a, N, c, 7000);
    showAll(c, k, "Чартер > 7000");

    saveTxt(a, N, "flights.txt");
    readCost(a, N, "prices.txt");
    showAll(a, N, "После чтения prices.txt");

    saveBin(a, N, "flights.dat");

    Flight d[N]{};
    int q = readBin(d, "flights.dat");
    showAll(d, q, "Из бинарного файла");

    return 0;
}