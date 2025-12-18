#include "functions.h"
#include <iostream>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::make_shared;

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
            cout << "Сохранено" << endl;
        }
        else if (choice == 4)
        {
            load_from_file(npcs);
            cout << "Загружено" << endl;
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

            npcs.erase(std::remove_if(npcs.begin(), npcs.end(),
                [](auto& n){ return !n->is_alive(); }), npcs.end());
        }

    } while (choice != 0);

    return 0;
}