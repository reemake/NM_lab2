#include <iostream>
#include <fstream>
using namespace std;

int GetDataFromFile(ifstream& file, int& n, float*& x, float*& y, float& xx, float& A, float& B) {
	if (!file.is_open())
	{
		cerr << "\tНевозможно открыть файл\n\n\n\n" << endl;
		system("pause");
		exit(1);
	}
	else {
		file >> n;
		if (n < 2) return 1;						// IER = 1

		x = new float[n];							// заполнение вектора x из входного файла
		for (int i = 0; i < n; i++)
			x[i] = -99999;
		int counter = 0;
		while (counter != n) {
			file >> x[counter++];
		}

		y = new float[n];							// заполнение вектора y из входного файла
		for (int i = 0; i < n; i++)
			y[i] = 0;
		int counter1 = 0;
		while (counter1 != n) {
			file >> y[counter1++];
		}

		file >> A >> B >> xx;
	}
	return 0;
}

void SetDataToFile(ofstream& file, int n, float xx, float yy)
{
	
	file << "IER = 0 - нет ошибки\nРезультат:" << endl;
	file << "Вычисленное интерполяционное функции в точке xx = " << xx << ": " << "yy = " << yy << endl;
}

bool xx_in_x(float xx, float* x, int n) {
	return (xx >= x[0] && xx <= x[n-1]);
}

int get_segment_number(float xx, float* x, int n) {
	int i = 1;
	while (xx > x[i]) 
		i++;
	return i;
}

float calcF(int k, float xx, float* x, float* a, float* b, float* c, float* d) {
	float yy = a[k] + b[k] * (xx - x[k]) + c[k] / 2 * (xx - x[k]) * (xx - x[k]) + d[k] / 6 * (xx - x[k]) * (xx - x[k]) * (xx - x[k]);
	return yy;
}

void progonka(float* aa, float* bb, float* cc, float* FF, float*& z, int n) 
{
	float* xi = new float[n];
	float* v = new float[n];
	v[0] = FF[0] / cc[0];
	xi[0] = -bb[0] / cc[0];

	for (int i = 1; i < n; i++) {
		float znam = cc[i] + aa[i] * xi[i - 1];
		xi[i] = -bb[i] / znam;
		v[i] = (FF[i] - aa[i] * v[i - 1]) / znam;
	}

	z[n - 1] = (v[n - 1] + xi[n - 1] * v[n - 1]) / (1 - xi[n - 1] * xi[n - 2]);
	for (int i = 2; i <= n; i++)
		z[n - i] = v[n - i] + xi[n - i] * z[n - i + 1];
}

void coef_spline(float* h, float* f, float*& a, float*& b, float*& c, float*& d, float A, float B, int n) 
{
	float* aa = new float[n];
	float* bb = new float[n];
	float* cc = new float[n];
	float* FF = new float[n];

	for (int i = 1; i < n - 1; i++) {
		aa[i] = h[i];
		bb[i] = h[i + 1];
		cc[i] = 2 * (h[i] + h[i + 1]);
		FF[i] = 6 * ((f[i + 1] - f[i]) / h[i + 1] - (f[i] - f[i - 1]) / h[i]);
	}
	aa[n-1] = 0.5;
	bb[0] = 0.5;
	cc[0] = cc[n-1] = 1;
	FF[0] = A;
	FF[n - 1] = 3 / h[n - 1] * (B - (f[n - 1] - f[n - 2]) / h[n - 1]);

	progonka(aa, bb, cc, FF, c, n);

	for (int i = 1; i < n; i++) {
		a[i] = f[i];
		d[i] = (c[i] - c[i - 1]) / h[i];
		b[i] = c[i] / 2 * h[i] - d[i] / 6 * h[i] * h[i] + (f[i] - f[i - 1]) / h[i];
	}
}


int main()
{
	int	n;
	float* x;
	float* y;
	float xx;
	float yy;
	float A;
	float B;
	ifstream file_input("in.txt");
	ofstream file_output("out.txt", ios::app);

	setlocale(LC_ALL, "RUS");

	cout << "\n\n\n\tЛабораторная работа №1:\n\n\tВычисление интерполяционного значения таблично заданной функции f(x) в заданной точке с помощью кубического сплайна.\n";
	cout << "\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\tПроверила: Шабунина З.А.\n\t\t\t\t\t\t\tПодготовил: Сафонов Н.С.\n\n\n\n\n\n\n\n\n";

	int IER = GetDataFromFile(file_input, n, x, y, xx, A, B);

	if (IER == 1) {
		file_output << "IER = 1 - кубический сплайн не может быть построен (n < 2)" << endl;
		cout << "\tIER = 1 - кубический сплайн не может быть построен (n < 2)\n\n\n\n" << endl;
	} 

	if (IER == 0 && xx_in_x(xx, x, n))
	{
		float* h = new float[n];
		for (int i = 1; i < n; i++) {
			h[i] = x[i] - x[i - 1];
			if (h[i] <= 0) {
				IER = 2;
				break;
			}
		}
		if (IER == 2) {
				file_output << "IER = 2 - нарушен порядок возрастания аргумента во входном векторе x" << endl;
				cout << "\tIER = 2 - нарушен порядок возрастания аргумента во входном векторе x\n\n\n" << endl;
		}
		else {

			float* a = new float[n];
			float* b = new float[n];
			float* c = new float[n];
			float* d = new float[n];

			coef_spline(h, y, a, b, c, d, A, B, n);

			int k = get_segment_number(xx, x, n);				// k -  номер отрезка, которрому принадлежит xx
			yy = calcF(k, xx, x, a, b, c, d);
			SetDataToFile(file_output, n, xx, yy);				// запись результатов в выходной файл
			cout << "\tIER = 0 - нет ошибки\n\n\n";

		}
	}
	else
	{
		file_output << "IER = 3 - точка xx не принадлежит отрезку [x0, xn]" << endl;
		cout << "\tIER = 3 - точка xx не принадлежит отрезку [x0, xn]\n\n\n" << endl;
	}
	return 0;
}