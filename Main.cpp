#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

vector<vector<vector<int>>> population;
int population_size;
int genes_size;
int lab_size;

void graphics() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Title");
	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}
};

//karakteristike gena (bloka labirinta)
vector<int> initCharacteristics() {
	vector<int> chars;
	chars.push_back(rand() % (lab_size));  // pozicijaX [0 - (lab_size-1)]
	chars.push_back(rand() % (lab_size));  // pozicijaY [0 - (lab_size-1)]
	chars.push_back(rand() % 2);			   // horizontalan/vertikalan [1/0]

	int max = chars[2] ? max = lab_size - 1 - chars[1] : max = lab_size - 1 - chars[0];
	chars.push_back(rand() % (max + 1)); // velicina bloka [0 - max(ovisno o poziciji i je li horizontalan/vertikalan)]
	return chars;
}

vector<vector<int>> initGenes() {
	vector<vector<int>> genes;
	for (int i = 0; i < genes_size; i++) {
		genes.push_back(initCharacteristics());
	}
	return genes;
}

void initPopulation() {
	for (int i = 0; i < population_size; i++) {
		population.push_back(initGenes());
	}

	//print population
	
	for (int i = 0; i < population_size; i++) {
		cout << "\npopulation:" << i << " genes: ";
		for (int j = 0; j < genes_size; j++) {
			cout << "(";
			for (int k = 0; k < 4; k++) {
				cout << population[i][j][k];
			}
			cout << ")";
		}
		break;
	}
	
};
vector<vector<bool>> generateMaze(vector<vector<int>> genes) {
	vector<vector<bool>> maze(lab_size, vector<bool>(lab_size, 0));

	for (int i = 0; i < genes_size; i++) {
		maze[genes[i][0]][genes[i][1]] = 1;

		if (genes[i][2]) { //horizontalan blok
			for (int j = 0; j <= genes[i][3]; j++) {
				maze[genes[i][0]][genes[i][1] + j] = 1;
			}
		} else { //vertikalan blok
			for (int j = 0; j <= genes[i][3]; j++) {
				maze[genes[i][0] + j][genes[i][1]] = 1;
			}
		}
	}

	//add START and END
	maze[0][1] = 0;
	maze[lab_size - 2][lab_size - 1] = 0;

	return maze;
};

void printMaze(vector<vector<bool>> maze) {
	cout << "\n\nLabirint:\n";
	for (int i = 0; i < lab_size; i++) {
		for (int j = 0; j < lab_size; j++) {
			cout << maze[i][j] << "  ";
		}
		cout << "\n";
	}
}

int countLabirynthFloor(vector<vector<bool>> maze) {
	int walls = 0;
	for (int i = 0; i < lab_size; i++) {
		for (int j = 0; j < lab_size; j++) {
			if (maze[i][j] == 1) walls++;
		}
	}
	int floors = (lab_size*lab_size) - walls;
	return floors;
}

bool is_finishable(vector<vector<bool>> maze) {
	vector<tuple<int, int>> stackit;
	tuple<int, int> position;
	int floors = countLabirynthFloor(maze);

	stackit.push_back(tuple<int, int>(0, 1)); //START

	int x, y, counter = 0;

	while (stackit.size() != 0) {
		position = stackit.back();
		stackit.pop_back();
		int oldSize = stackit.size();

		x = get<0>(position);
		y = get<1>(position);

		if (maze[x][y]) continue;
		maze[x][y] = 1;

		if (x - 1 >= 0 && y - 1 >= 0)				if (!maze[x - 1][y - 1])	stackit.push_back(tuple<int, int>(x - 1, y - 1));
		if (y - 1 >= 0)								if (!maze[x][y - 1])		stackit.push_back(tuple<int, int>(x, y - 1));
		if (x - 1 >= 0)								if (!maze[x - 1][y])		stackit.push_back(tuple<int, int>(x - 1, y));
		if (x + 1 < lab_size && y + 1 < lab_size)	if (!maze[x + 1][y + 1])	stackit.push_back(tuple<int, int>(x + 1, y + 1));
		if (x + 1 < lab_size)						if (!maze[x + 1][y])		stackit.push_back(tuple<int, int>(x + 1, y));
		if (y + 1 < lab_size)						if (!maze[x][y + 1])		stackit.push_back(tuple<int, int>(x , y + 1));
		if (x - 1 >= 0 && y + 1 < lab_size)			if (!maze[x - 1][y + 1])	stackit.push_back(tuple<int, int>(x - 1, y + 1));
		if (x + 1 < lab_size && y - 1 >= 0)			if (!maze[x + 1][y - 1])	stackit.push_back(tuple<int, int>(x + 1, y - 1));

		counter++;
	};

	cout << counter << "  " << floors << "\n";
	return counter == floors;
};

double horizontal_vertical_ratio(vector<vector<int>> genes) {
	int horizontal_square_number = 0;
	int vertical_square_number = 0;

	for (int i = 0; i < genes_size; i++) {
		genes[i][3] ? horizontal_square_number++ : vertical_square_number++;
	}

	return double(horizontal_square_number) / double(vertical_square_number);
}

double homogeneity_factor(vector<vector<bool>> maze) { //ovo vjv nije ovo
	int blocks, floors;
	floors = countLabirynthFloor(maze);
	blocks = lab_size - floors;
	if (floors < blocks) {
		return (double)floors / (double)blocks;
	}
	else {
		return (double)blocks / (double)floors;
	}
}

double intersected_block_ratio(vector<vector<bool>> maze) {
	int mazeBlocks, intersectedBlocks = 0;
	mazeBlocks = lab_size ^ 2 - countLabirynthFloor(maze);

	for (int i = 1; i < lab_size - 1; i++) {
		for (int j = 1; j < lab_size - 1; j++) {
			if (!maze[i][j]) continue;
			if (maze[i + 1][j] && maze[i][j + 1] && maze[i][j - 1] && maze[i - 1][j]) intersectedBlocks++;
		}
	}
	return (double)intersectedBlocks / double(mazeBlocks);
}

double fitness(vector<vector<int>> genes, vector<vector<bool>> maze) {
	double fScore;
	fScore = 0.4 * is_finishable(maze) - 0.1 * intersected_block_ratio(maze) + 0.2 * horizontal_vertical_ratio(genes);

	//cout << "\n\n Is finishable: " << is_finishable(maze);
	return fScore;
};

int main()
{
	cout << "Population size:" << endl;
	cin >> population_size;

	cout << "Gene size:" << endl;
	cin >> genes_size;

	cout << "Labyrinth size:" << endl;
	cin >> lab_size;

	initPopulation();

	vector<vector<int>> genes = population[0];
	vector<vector<bool>> maze = generateMaze(genes);
	
	printMaze(maze);
	fitness(genes, maze);

	graphics();
	return 0;
}


