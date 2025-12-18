#include "functions.h"
#include <cmath>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::make_shared;

//================ Observer =================
void ConsoleObserver::on_kill(const string& killer, const string& victim)
{
    cout << "[LOG] " << killer << " killed " << victim << endl;
}

FileObserver::FileObserver(const string& filename)
{
    file = new std::ofstream(filename, std::ios::app);
}

void FileObserver::on_kill(const string& killer, const string& victim)
{
    (*file) << killer << " killed " << victim << endl;
}

//================ NPC ======================
NPC::NPC(const string& n, int px, int py)
    : name(n), x(px), y(py), alive(true) {}

double NPC::distance_to(const NPC& o) const
{
    return std::sqrt((x - o.x)*(x - o.x) + (y - o.y)*(y - o.y));
}

bool NPC::is_alive() const { return alive; }
void NPC::kill() { alive = false; }
string NPC::get_name() const { return name; }
int NPC::get_x() const { return x; }
int NPC::get_y() const { return y; }

//================ NPC types ================
Orc::Orc(const string& n, int x, int y) : NPC(n, x, y) {}
string Orc::type() const { return "Orc"; }
void Orc::accept(Visitor& v) { v.visit(*this); }

Bear::Bear(const string& n, int x, int y) : NPC(n, x, y) {}
string Bear::type() const { return "Bear"; }
void Bear::accept(Visitor& v) { v.visit(*this); }

Squirrel::Squirrel(const string& n, int x, int y) : NPC(n, x, y) {}
string Squirrel::type() const { return "Squirrel"; }
void Squirrel::accept(Visitor& v) { v.visit(*this); }

//================ Factory ==================
shared_ptr<NPC> NPCFactory::create(const string& type,
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

//================ Battle ===================
BattleVisitor::BattleVisitor(NPC& a,
                             vector<shared_ptr<Observer>>& o)
    : attacker(a), observers(o) {}

void BattleVisitor::notify(const string& victim)
{
    for (auto& obs : observers)
        obs->on_kill(attacker.get_name(), victim);
}

void BattleVisitor::visit(Orc& npc)
{
    if (attacker.type() == "Orc" || attacker.type() == "Bear")
    {
        npc.kill(); notify(npc.get_name());
    }
}

void BattleVisitor::visit(Bear& npc)
{
    if (attacker.type() == "Orc")
    {
        npc.kill(); notify(npc.get_name());
    }
}

void BattleVisitor::visit(Squirrel&) {}

//================ File ops =================
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
