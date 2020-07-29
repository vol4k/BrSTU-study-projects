#include <Windows.h>
#include <string>
using namespace std;

class pinv {
public:
	static void transform(const string&, BYTE, const char&);
private:
	static bool is_BMP(const string&);
	static FILE* getPictureHandle(const string&);
	static FILE* getDropHandle(string);
	static void inverseColorBits(const string&, const BYTE&, const char&);
};