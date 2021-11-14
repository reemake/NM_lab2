#include <iostream>
#include <fstream>
using namespace std;

int getDataFromFile(ifstream& file, int& n, float*& x, float*& y, float& xx, float& A, float& B) {
	if (!file.is_open())
	{
		cerr << "Невозможно открыть файл\n\n\n\n" << endl;
		system("pause");
		exit(1);
	}
	else {
		file >> n;
		if (n < 2) return 1;						// IER = 1

		x = new float[n+1];							// заполнение вектора x из входного файла
		int counter = 0;
		while (counter != n + 1) {
			file >> x[counter++];
		}

		y = new float[n+1];							// заполнение вектора y из входного файла
		int counter1 = 0;
		while (counter1 != n + 1) {
			file >> y[counter1++];
		}

		file >> A >> B >> xx;
	}
	return 0;										// IER = 0
}

void setDataToFile(ofstream& file, float xx, float yy)
{
	
	file << "IER = 0 - нет ошибки\nРезультат:" << endl;
	file << "Для xx = " << xx << ": " << "yy = " << yy << endl;
}

bool xx_in_x(float xx, float* x, int n) {
	return (xx >= x[0] && xx <= x[n]);
}

int getSegmentNumber(float xx, float* x) {
	int i = 1;
	while (xx > x[i]) 
		i++;
	return i;
}

float calcS(int k, float xx, float* x, float* a, float* b, float* c, float* d) {
	float yy = a[k] + b[k] * (xx - x[k]) + c[k] / 2.0f * (xx - x[k]) * (xx - x[k]) + d[k] / 6.0f * (xx - x[k]) * (xx - x[k]) * (xx - x[k]);
	return yy;
}

void progonka(float* aa, float* bb, float* cc, float* FF, float*& z, int n) 
{
	float* xi = new float[n+1];
	float* v = new float[n+1];
	v[1] = -1.0f * FF[0] / cc[0];
	xi[1] = -1.0f * bb[0] / cc[0];

	v[0] = 0.0f;       
	xi[0] = 0.0f;        

	for (int i = 0; i < n; i++) {               
		float znam = cc[i] + aa[i] * xi[i];      
		xi[i+1] = -1.0f * bb[i] / znam;
		v[i+1] = 1.0f * (FF[i] - aa[i] * v[i]) / znam;      
	}
	z[n] = (FF[n] - aa[n] * v[n]) / (cc[n] + aa[n] * xi[n]);
	for (int i = 1; i < n + 1; i++)
		z[n - i] = v[n - i + 1] + xi[n - i + 1] * z[n - i + 1];
}

void coefSpline(float* h, float* f, float*& a, float*& b, float*& c, float*& d, float A, float B, int n) 
{
	float* aa = new float[n + 1];
	float* bb = new float[n + 1];
	float* cc = new float[n + 1];
	float* FF = new float[n + 1];

	for (int i = 1; i < n; i++) {
		aa[i] = h[i];
		bb[i] = h[i + 1];
		cc[i] = 2.0f * (h[i] + h[i + 1]);
		FF[i] = 6.0f * ((f[i + 1] - f[i]) / h[i + 1] - (f[i] - f[i - 1]) / h[i]);
	}         
	aa[n] = 0.5f;
	bb[0] = 0.0f;
	cc[0] = cc[n] = 1.0f;

	FF[0] = 1.0f * A;												   // левая граница
	FF[n] = 3.0f / h[n] * (B - (f[n] - f[n - 1]) / h[n]);              // правая граница

	progonka(aa, bb, cc, FF, c, n);

	for (int i = 1; i < n + 1; i++) {
		a[i] = f[i];
		d[i] = (c[i] - c[i - 1]) / h[i];
		b[i] = c[i] / 2.0f * h[i] - d[i] / 6.0f * h[i] * h[i] + (f[i] - f[i - 1]) / h[i];
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
	ifstream fileInput("test4.txt");
	ofstream fileOutput("out.txt");

	setlocale(LC_ALL, "RUS");

	cout << "Лабораторная работа №1:\n\nВычисление интерполяционного значения таблично заданной функции f(x) в заданной точке с помощью кубического сплайна.\n";
	cout << "Проверила: Шабунина З.А.\nПодготовил: Сафонов Н.С.\n\n\n";

	int IER = getDataFromFile(fileInput, n, x, y, xx, A, B);

	if (IER == 1) {
		fileOutput << "IER = 1 - кубический сплайн не может быть построен (n < 2)" << endl;
		cout << "IER = 1 - кубический сплайн не может быть построен (n < 2)\n\n\n" << endl;
	} 

	else if (IER == 0 && xx_in_x(xx, x, n))
	{
		float* h = new float[n + 1];
		h[0] = 0;
		for (int i = 1; i < n + 1; i++) {
			h[i] = x[i] - x[i - 1];
			if (h[i] <= 0) {
				IER = 2;
				break;
			}
		}

		if (IER == 2) {
				fileOutput << "IER = 2 - нарушен порядок возрастания аргумента во входном векторе x" << endl;
				cout << "IER = 2 - нарушен порядок возрастания аргумента во входном векторе x\n\n\n" << endl;
		}
		else {

			float* a = new float[n+1];
			float* b = new float[n+1];
			float* c = new float[n+1];
			float* d = new float[n+1];

			coefSpline(h, y, a, b, c, d, A, B, n);

			int k = getSegmentNumber(xx, x);					// k -  номер отрезка, которрому принадлежит xx
			yy = calcS(k, xx, x, a, b, c, d);
			setDataToFile(fileOutput, xx, yy);					// запись результатов в выходной файл
			cout << "IER = 0 - нет ошибки\n\n\n";

		}
	}

	else
	{
		fileOutput << "IER = 3 - точка xx не принадлежит отрезку [x0, xn]" << endl;
		cout << "IER = 3 - точка xx не принадлежит отрезку [x0, xn]\n\n\n" << endl;
	}

	return 0;
}