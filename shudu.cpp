#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

#define debugmode

constexpr int N = 9; // 数独维度
constexpr int M = 81; // 数独格子数量
constexpr int K = 50; // 数独随机填充次数
constexpr int MAXN = 5000; // 数独生成最大尝试次数

bool isSafe(int grid[M], int i, int num); //判断数据是否合法辅助函数
bool solveHelper(int grid[M], int i); //数独求解辅助函数
bool solveSudoku(int grid[M]); //数独求解函数

void generateSudoku(int gameCount, const char* filename);//-c参数生成函数
void solveSudokuFile(const char* inFilename, const char* outFilename);//-s参数求解函数

int main(int argc, const char* argv[]) {
	if (argc != 3 || argv == NULL) {
		cout << "Usage: shudu.exe -c <gameCount> | -s <inputFile>" << endl;
		return 1;
	}

	if (string(argv[1]) == "-c") {
		const int gameCount = atoi(argv[2]);
		if (gameCount > 0 && gameCount < 1000001)
			generateSudoku(gameCount, "game.txt");
		else
		{
			cout << "arg must in 1-1000000" << endl;
			return 1;
		}
	}
	else if (string(argv[1]) == "-s") {
		solveSudokuFile(argv[2], "sudoku.txt");
	}
	else {
		cout << "Invalid arguments" << endl;
		return 1;
	}

	return 0;
}

// 判断在(i, j)位置填入num是否合法
bool isSafe(int grid[M], int i, int num) {
	int row = i / N;
	int col = i % N;

	// 检查行是否合法
	for (int j = 0; j < N; j++) {
		if (grid[row * N + j] == num) {
			return false;
		}
	}

	// 检查列是否合法
	for (int j = 0; j < N; j++) {
		if (grid[col + j * N] == num) {
			return false;
		}
	}

	// 检查3x3宫是否合法
	int boxRow = row / 3;
	int boxCol = col / 3;
	for (int j = boxRow * 3; j < boxRow * 3 + 3; j++) {
		for (int k = boxCol * 3; k < boxCol * 3 + 3; k++) {
			if (grid[j * N + k] == num) {
				return false;
			}
		}
	}

	return true;
}

// 递归求解数独
bool solveHelper(int grid[M], int i) {
	if (i == M) {
		return true;
	}

	if (grid[i] != 0) {
		return solveHelper(grid, i + 1);
	}

	for (int num = 1; num <= N; num++) {
		if (isSafe(grid, i, num)) {
			grid[i] = num;
			if (solveHelper(grid, i + 1)) {
				return true;
			}
			grid[i] = 0;
		}
	}

	return false;
}

// 求解数独
bool solveSudoku(int grid[M]) {
	return solveHelper(grid, 0);
}

// 生成数独
void generateSudoku(int gameCount, const char* filename) {
#ifdef debugmode
	cout << "begin function generateSudoku" << endl;
#endif
	//文件流连接输出文件
	ofstream outFile(filename);
	if (!outFile) {
		cout << "Unable to open file " << filename << endl;
		return;
	}

	srand((unsigned)time(NULL));
	int count = 0;
	while (count < gameCount) {
		/*
		生成随机数独思路：
		1.初始为全0
		2.生成随机数，判断当前是否可填入，如果可填入，则填入，下标i++；如果不可填入，重新生成随机数，下标不变
		3.不论是否成功填入，trycount均++，当生成一个棋盘过程中trycount超过最大尝试次数，判断为棋盘无解，重新生成，防止程序卡死
		4.同时满足i==M和solveSudoku(grid)即为数独成功生成且有解，输出时判断当前位置是否和四个随机数相等，如果相等，则抹除输出为$，如果不等，输出原本数值
		*/
#ifdef debugmode
		cout << "now count=" << count << endl;
#endif
		int grid[M] = { 0 };
		int i = 0;
		int tryCount = 0;
		while (i < M) {
			if (tryCount > MAXN) {
				break;
			}
			int num = rand() % N + 1;
			if (isSafe(grid, i, num)) {
				grid[i] = num;
				i++;
			}
			tryCount++;
		}

		if (i == M && solveSudoku(grid)) {
			count++;
			for (int j = 0; j < M; j++) {
				//判断四次随机数是为了宏观上$数量约占总体的九分之四，即一个输出题目约有40个未知位置
				int randj1 = rand() % N + 1;
				int randj2 = rand() % N + 1;
				int randj3 = rand() % N + 1;
				int randj4 = rand() % N + 1;
				if (grid[j] == randj1) {
					outFile << "$";
				}
				else if (grid[j] == randj2) {
					outFile << "$";
				}
				else if (grid[j] == randj3) {
					outFile << "$";
				}
				else if (grid[j] == randj4) {
					outFile << "$";
				}
				else {
					outFile << grid[j];
				}
			}
			outFile << endl;
			outFile << endl;
		}
	}
	outFile.close();
	cout << "Successfully generated " << gameCount << " sudoku games in " << filename << endl;
}

// 从文件中读取数独并求解
void solveSudokuFile(const char* inFilename, const char* outFilename) {
#ifdef debugmode
	cout << "begin function solveSudokuFile" << endl;
#endif
	ifstream inFile(inFilename);
	if (!inFile) {
		cout << "Unable to open file " << inFilename << endl;
		return;
	}

	ofstream outFile(outFilename);
	if (!outFile) {
		cout << "Unable to open file " << outFilename << endl;
		return;
	}
	string line;
	int count = 0;
	while (getline(inFile, line)) {
		if (line.empty()) {
			continue;
		}

		if (line.length() != M) {
			cout << "Invalid input file format" << endl;
			return;
		}

		int grid[M] = { 0 };
		for (int i = 0; i < M; i++) {
			if (line[i] == '$') {
				grid[i] = 0;
			}
			else {
				grid[i] = line[i] - '0';
			}
		}

		if (solveSudoku(grid)) {
			count++;
			for (int i = 0; i < M; i++) {
				outFile << grid[i];
				if ((i + 1) % N == 0) {
					outFile << endl;
				}
			}
			outFile << endl;
		}
	}
	inFile.close();
	outFile.close();

	cout << "Successfully solved " << count << " sudoku games in " << inFilename << " and saved the answers in " << outFilename << endl;
}