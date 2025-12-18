#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>

using std::string;
using std::vector;
using std::shared_ptr;

//================ Observer ================
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void on_kill(const string& killer, const string& victim) = 0;
};

class ConsoleObserver : public Observer
{
public:
    void on_kill(const string& killer, const string& victim) override;
};

class FileObserver : public Observer
{
public:
    explicit FileObserver(const string& filename);
    void on_kill(const string& killer, const string& victim) override;

private:
    std::ofstream* file;
};

//================ Visitor ================
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

//================ NPC ====================
class NPC
{
protected:
    string name;
    int x;
    int y;
    bool alive;

public:
    NPC(const string& name, int x, int y);
    virtual ~NPC() = default;

    virtual string type() const = 0;
    virtual void accept(Visitor& v) = 0;

    double distance_to(const NPC& other) const;
    bool is_alive() const;
    void kill();

    string get_name() const;
    int get_x() const;
    int get_y() const;
};

class Orc : public NPC
{
public:
    Orc(const string& name, int x, int y);
    string type() const override;
    void accept(Visitor& v) override;
};

class Bear : public NPC
{
public:
    Bear(const string& name, int x, int y);
    string type() const override;
    void accept(Visitor& v) override;
};

class Squirrel : public NPC
{
public:
    Squirrel(const string& name, int x, int y);
    string type() const override;
    void accept(Visitor& v) override;
};

//================ Factory =================
class NPCFactory
{
public:
    static shared_ptr<NPC> create(const string& type,
                                  const string& name,
                                  int x,
                                  int y);
};

//================ Battle ==================
class BattleVisitor : public Visitor
{
public:
    BattleVisitor(NPC& attacker,
                  vector<shared_ptr<Observer>>& observers);

    void visit(Orc&) override;
    void visit(Bear&) override;
    void visit(Squirrel&) override;

private:
    NPC& attacker;
    vector<shared_ptr<Observer>>& observers;
    void notify(const string& victim);
};

//================ File ops ================
void save_to_file(const vector<shared_ptr<NPC>>& npcs);
void load_from_file(vector<shared_ptr<NPC>>& npcs);

#endif // FUNCTIONS_H
