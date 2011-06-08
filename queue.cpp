#include <cstdio>
#include <iostream>
#include <queue>

using namespace std;

int main()
{
	queue<float *> q;
	unsigned int a, b;
	float *t;

	for (a = 0; a < 8; a++) {
		t = new float[3];
		if (t == NULL)
			break;
		for (b = 0; b < 3; b++)
			t[b] = 1 - 2*!!(a & (1 << b));
		cout << "[" << t[0] << ", " << t[1] << ", " << t[2] << "]\n";
		q.push(t);
	}

	while (!q.empty()) {
		t = q.front();
		cout << q.size() << ": [" << t[0] << ", " << t[1] << ", " <<
			t[2] << "]\n";
		delete[] t;
		q.pop();
	}
}
