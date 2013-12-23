#include "FreeImage.h"
#include "GLUT/glut.h"
#include "Eigen/Core"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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
	const char* delims = " ,|";
	while (!infile.eof())
	{
		infile.getline(line, 1023);
		string linestr = string(line);
		if (doneHeaders && !linestr.empty())
		{
			string id = string(strtok(line, delims));
			string temp_str = string(strtok(NULL, delims));
			double temp = atof(temp_str.c_str());
			double score = atof(strtok(NULL, delims));
			if (whichTemps.count(temp) > 0)
			{
				// Assuming 5 colors
				VectorXd colors(15);
				for (int i = 0; i < 5; i++)
				{
					colors[3*i] = atof(strtok(NULL, delims));
					colors[3*i+1] = atof(strtok(NULL, delims));
					colors[3*i+2] = atof(strtok(NULL, delims));
				}
				string imageFilename = patternIDPath + string("/") + temp_str + string("/") + id + string(".png");
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
	cout << "Loading data..." << flush;
	char* patternIDPath = argv[1];
	unordered_set<double> whichTemps;
	whichTemps.insert(0.05);
	vector<SampleRecord> sampleRecords;
	loadSamples(patternIDPath, whichTemps, sampleRecords);
	cout << "DONE (" << sampleRecords.size() << " samples loaded)." << endl;

	// Find and project islands

	FreeImage_DeInitialise();
	return 0;
}




