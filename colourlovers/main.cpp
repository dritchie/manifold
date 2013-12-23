#include "FreeImage.h"
#include "GLUT/glut.h"
#include "Eigen/Core"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <tr1/unordered_set>

using namespace std;
using namespace std::tr1;
using namespace Eigen;

struct SampleRecord
{
	double score;
	VectorXd colors;
	FIBITMAP* image;
};

void loadSamples(const string& patternIDPath, const unordered_set<double>& whichTemps, vector<SampleRecord>& outSamps)
{
	string filename = patternIDPath + string(".csv");
	ifstream infile(filename.c_str());
	char line[1024];
	bool doneHeaders = false;
	while (!infile.eof())
	{
		infile.getline(line, 1023);
		if (doneHeaders)
		{
			stringstream ss(line);
			string dummy;	// For commas, pipes, etc.
			string id;
			double temp;
			double score;
			ss >> id >> dummy >> temp >> dummy >> score >> dummy;
			if (whichTemps.count(temp))
			{
				// Assuming 5 colors
				VectorXd colors(15);
				for (int i = 0; i < 5; i++)
				{
					ss >> colors[3*i] >> colors[3*i+1] >> colors[3*i+2] >> dummy;
				}
				string imageFilename = patternIDPath + string("/") + id + string(".png");
				FIBITMAP* image = FreeImage_Load(FIF_PNG, imageFilename.c_str());
				SampleRecord srec;
				srec.score = score;
				srec.colors = colors;
				srec.image = image;
				outSamps.push_back(srec);
			}
		}
		else doneHeaders = true;
	}
}




int main(int argc, char** argv)
{
	FreeImage_Initialise();

	// Load data
	char* patternIDPath = argv[1];
	unordered_set<double> whichTemps;
	whichTemps.insert(0.05);
	vector<SampleRecord> sampleRecords;
	loadSamples(patternIDPath, whichTemps, sampleRecords);

	// Find and project islands

	FreeImage_DeInitialise();
	return 0;
}




