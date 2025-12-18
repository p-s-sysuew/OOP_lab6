

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

// Observer
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void on_kill(const string& killer, const string& victim) = 0;
};

class ConsoleObserver : public Observer
{
public:
    void on_kill(const string& killer, const string& victim) override
    {
        cout << "[LOG] " << killer << " killed " << victim << endl;
    }
};

class FileObserver : public Observer
{
    std::ofstream file;
public:
    explicit FileObserver(const string& filename)
        : file(filename, std::ios::app) {}

    void on_kill(const string& killer, const string& victim) override
    {
        file << killer << " killed " << victim << endl;
    }
};


// Visitor

class Orc;
class Bear;
class Squirrel;

class Visitor
{
public:
    virtual ~Visitor() = default;
    virtual void visit(Orc&) = 0;
    virtual void visit(Bear&) = 0;
    virtual void visit(Squirrel&) = 0;
};


// NPC base

class NPC
{
protected:
    string name;
    int x;
    int y;
    bool alive;

public:
    NPC(const string& n, int px, int py)
        : name(n), x(px), y(py), alive(true) {}

    virtual ~NPC() = default;
    virtual string type() const = 0;
    virtual void accept(Visitor& v) = 0;

    double distance_to(const NPC& other) const
    {
        return std::sqrt((x - other.x)*(x - other.x) +
                         (y - other.y)*(y - other.y));
    }

    bool is_alive() const { return alive; }
    void kill() { alive = false; }

    string get_name() const { return name; }
    int get_x() const { return x; }
    int get_y() const { return y; }
};

class Orc : public NPC
{
public:
    Orc(const string& n, int x, int y) : NPC(n, x, y) {}
    string type() const override { return "Orc"; }
    void accept(Visitor& v) override { v.visit(*this); }
};

class Bear : public NPC
{
public:
    Bear(const string& n, int x, int y) : NPC(n, x, y) {}
    string type() const override { return "Bear"; }
    void accept(Visitor& v) override { v.visit(*this); }
};

class Squirrel : public NPC
{
public:
    Squirrel(const string& n, int x, int y) : NPC(n, x, y) {}
    string type() const override { return "Squirrel"; }
    void accept(Visitor& v) override { v.visit(*this); }
};


// Factory

class NPCFactory
{
public:
    static shared_ptr<NPC> create(const string& type,
                                  const string& name,
                                  int x,
                                  int y)
    {
        if (x < 0 || x > 500 || y < 0 || y > 500)
            throw std::runtime_error("Координаты вне диапазона");

        if (type == "Orc") return make_shared<Orc>(name, x, y);
        if (type == "Bear") return make_shared<Bear>(name, x, y);
        if (type == "Squirrel") return make_shared<Squirrel>(name, x, y);

        throw std::runtime_error("Неизвестный тип NPC");
    }
};


// Battle Visitor

class BattleVisitor : public Visitor
{
    NPC& attacker;
    vector<shared_ptr<Observer>>& observers;

    void notify(const string& victim)
    {
        for (auto& o : observers)
            o->on_kill(attacker.get_name(), victim);
    }

public:
    BattleVisitor(NPC& a, vector<shared_ptr<Observer>>& o)
        : attacker(a), observers(o) {}

    void visit(Orc& npc) override
    {
        if (attacker.type() == "Orc" || attacker.type() == "Bear")
        {
            npc.kill(); notify(npc.get_name());
        }
    }

    void visit(Bear& npc) override
    {
        if (attacker.type() == "Orc")
        {
            npc.kill(); notify(npc.get_name());
        }
    }

    void visit(Squirrel&) override {}
};


// File operations

void save_to_file(const vector<shared_ptr<NPC>>& npcs)
{
    std::ofstream out("npcs.txt");
    for (auto& n : npcs)
        if (n->is_alive())
            out << n->type() << " " << n->get_name() << " "
                << n->get_x() << " " << n->get_y() << endl;
}

void load_from_file(vector<shared_ptr<NPC>>& npcs)
{
    std::ifstream in("npcs.txt");
    npcs.clear();
    string type, name;
    int x, y;
    while (in >> type >> name >> x >> y)
        npcs.push_back(NPCFactory::create(type, name, x, y));
}


// Menu
int main()
{
    vector<shared_ptr<NPC>> npcs;
    vector<shared_ptr<Observer>> observers{
        make_shared<ConsoleObserver>(),
        make_shared<FileObserver>("log.txt")
    };

    int choice;
    do
    {
        cout << "\n1 - Добавить NPC\n"
             << "2 - Показать NPC\n"
             << "3 - Сохранить\n"
             << "4 - Загрузить\n"
             << "5 - Запуск боя\n"
             << "0 - Выход\n"
             << "Выбор: ";
        cin >> choice;

        if (choice == 1)
        {
            string type, name;
            int x, y;
            cout << "Тип (Orc/Bear/Squirrel): "; cin >> type;
            cout << "Имя: "; cin >> name;
            cout << "x y: "; cin >> x >> y;
            npcs.push_back(NPCFactory::create(type, name, x, y));
        }
        else if (choice == 2)
        {
            for (auto& n : npcs)
                cout << n->type() << " " << n->get_name()
                     << " (" << n->get_x() << "," << n->get_y() << ")" << endl;
        }
        else if (choice == 3)
        {
            save_to_file(npcs);
            cout << "Сохранено в файл npcs.txt" << endl;
        }
        else if (choice == 4)
        {
            load_from_file(npcs);
            cout << "Загружено из файла npcs.txt" << endl;
        }
        else if (choice == 5)
        {
            double range;
            cout << "Дальность боя: "; cin >> range;
            for (auto& a : npcs)
                for (auto& b : npcs)
                    if (a != b && a->is_alive() && b->is_alive() &&
                        a->distance_to(*b) <= range)
                    {
                        BattleVisitor v(*a, observers);
                        b->accept(v);
                    }

            npcs.erase(remove_if(npcs.begin(), npcs.end(),
                                 [](auto& n){ return !n->is_alive(); }),
                       npcs.end());
        }

    } while (choice != 0);

    return 0;
}
