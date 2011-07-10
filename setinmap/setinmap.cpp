#include <iostream>
#include <map>
#include <set>

int main()
{
	typedef std::set<int> Set;
	typedef std::map<int, Set> Map;
	Map map;

	map[0].insert(1);
	map[0].insert(2);
	map[0].insert(3);
	map[5].insert(5);

	for (Map::const_iterator I = map.begin(), E = map.end(); I != E; I++) {
		std::cout << I->first << ":" << std::endl;
		Set s = I->second;
		for (Set::const_iterator II = s.begin(), EE = s.end(); II != EE; II++)
			std::cout << "  " << *II << std::endl;

	}

	return 0;
}
