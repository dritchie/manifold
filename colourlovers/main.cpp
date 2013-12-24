#include "FreeImage.h"
#include "GLUT/glut.h"
#include "Eigen/Core"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <tr1/unordered_set>
#include <limits>

#include "KNN.h"
#include "lle.h"

using namespace std;
using namespace std::tr1;
using namespace Eigen;

struct SampleRecord
{
	double score;
	VectorXd colors;
	FIBITMAP* image;
	VectorXd embedding;
};

// GLUT globals and constants
int windows[2];
int mapWindowSize = 500;
int imWindowSize = 200;
vector<SampleRecord> sampleRecords;
vector< vector<int> > islands;
int whichIsland;
int whichPoint;
double minx, maxx, miny, maxy;
KNNBruteForce mapKNN;

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

void mapManifolds(int k, vector<SampleRecord>& samps, vector< vector<int> >& islands)
{
	// Extract data
	int dim = samps[0].colors.size();
	MatrixXd data(dim, samps.size());
	for (size_t i = 0; i < samps.size(); i++)
		data.col(i) = samps[i].colors;

	// Find islands
	cout << "Finding islands..." << flush;
	KNNBruteForce knn(data);
	Graph g(samps.size());
	knn.buildGraph(k, g);
	g.symmetrize();
	g.connectedComponents(islands);
	cout << "DONE (" << islands.size() << " found)." << endl;

	// Do embeddings for each island
	for (size_t i = 0; i < islands.size(); i++)
	{
		const vector<int>& island = islands[i];
		cout << "Computing embedding " << i+1 <<  " (size " << island.size() << ")..." << flush;
		MatrixXd inData(dim, island.size());
		for (size_t j = 0; j < island.size(); j++)
			inData.col(j) = data.col(island[j]);
		MatrixXd outData;
		lle(inData, outData, 2, k);
		for (size_t j = 0; j < island.size(); j++)
			samps[island[j]].embedding = outData.col(j);
		cout << "DONE." << endl;
	}
}

/////////////////////////////////////////////

void postRedisplayBothWindows()
{
	int currWindow = glutGetWindow();
	glutSetWindow(windows[0]);
	glutPostRedisplay();
	glutSetWindow(windows[1]);
	glutPostRedisplay();
	glutSetWindow(currWindow);
}

/////////////////////////////////////////////

void rebuildMapKNN()
{
	mapKNN.clear();
	const vector<int>& island = islands[whichIsland];
	for (size_t i = 0; i < island.size(); i++)
	{
		const VectorXd& p = sampleRecords[island[i]].embedding;
		mapKNN.addPoint(p);
	}
}

void fitMapViewToData()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	minx = numeric_limits<double>::infinity();
	maxx = -numeric_limits<double>::infinity();
	miny = numeric_limits<double>::infinity();
	maxy = -numeric_limits<double>::infinity();
	const vector<int>& island = islands[whichIsland];
	for (size_t i = 0; i < island.size(); i++)
	{
		const VectorXd& p = sampleRecords[island[i]].embedding;
		minx = fmin(p.x(), minx);
		maxx = fmax(p.x(), maxx);
		miny = fmin(p.y(), miny);
		maxy = fmax(p.y(), maxy);
	}
	double xrange = fabs(maxx - minx);
	double yrange = fabs(maxy - miny);
	minx -= 0.05*xrange;
	maxx += 0.05*xrange;
	miny -= 0.05*yrange;
	maxy += 0.05*yrange;
	gluOrtho2D(minx, maxx, miny, maxy);
}

void resizeMap(int w, int h)
{
	glViewport(0, 0, w, h);
	fitMapViewToData();
	rebuildMapKNN();
}

void displayMap()
{
	glPointSize(4.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_POINTS);
	
	const vector<int>& island = islands[whichIsland];
	glColor3f(1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < island.size(); i++)
	{
		const VectorXd& p = sampleRecords[island[i]].embedding;
		glVertex2d(p.x(), p.y());
	}
	// Re-render selected point on top of everything else.
	glColor3f(1.0f, 0.0f, 0.0f);
	const VectorXd& p = sampleRecords[island[whichPoint]].embedding;
	glVertex2d(p.x(), p.y());

	glEnd();
	glutSwapBuffers();
}

void mouseClickMap(int button, int state, int x, int y)
{
	double xt = (double)x/mapWindowSize;
	double yt = (double)y/mapWindowSize;
	yt = 1 - yt;	// invert y
	double px = (1-xt)*minx + xt*maxx;
	double py = (1-yt)*miny + yt*maxy;
	VectorXd p(2); p.x() = px; p.y() = py;
	vector<int> nearest;
	mapKNN.kNearest(1, p, nearest);
	whichPoint = nearest[0];

	// glutPostRedisplay();
	postRedisplayBothWindows();
}

void keyboardMap(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'a':
			whichIsland = whichIsland - 1;
			if (whichIsland < 0) whichIsland = islands.size() - 1;
			whichPoint = 0;
			break;
		case 'd':
			whichIsland = (whichIsland + 1) % (islands.size());
			whichPoint = 0;
			break;
		default:
			break;
	}
	fitMapViewToData();
	rebuildMapKNN();
	// glutPostRedisplay();
	postRedisplayBothWindows();
}

void initManifoldMapWindow()
{
	glutInitWindowSize(mapWindowSize, mapWindowSize);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	windows[0] = glutCreateWindow("Manifold Map");
	glutDisplayFunc(displayMap);
	glutReshapeFunc(resizeMap);
	glutKeyboardFunc(keyboardMap);
	glutMouseFunc(mouseClickMap);
}

/////////////////////////////////////////////

void resizeIm(int w, int h)
{
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void displayIm()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	FIBITMAP* image = sampleRecords[islands[whichIsland][whichPoint]].image;
	unsigned char* bits = FreeImage_GetBits(image);
	glDrawPixels(imWindowSize, imWindowSize, GL_RGBA, GL_UNSIGNED_BYTE, bits);
	glutSwapBuffers();
}

void initImageWindow()
{
	glutInitWindowSize(imWindowSize, imWindowSize);
	glutInitWindowPosition(600, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	windows[1] = glutCreateWindow("Recoloring Preview");
	glutDisplayFunc(displayIm);
	glutReshapeFunc(resizeIm);
}

int main(int argc, char** argv)
{
	FreeImage_Initialise();

	// Load data
	cout << "Loading data..." << flush;
	char* patternIDPath = argv[1];
	unordered_set<double> whichTemps;
	whichTemps.insert(0.05);
	loadSamples(patternIDPath, whichTemps, sampleRecords);
	cout << "DONE (" << sampleRecords.size() << " samples loaded)." << endl;

	// Find and project islands
	int k = 10;
	mapManifolds(k, sampleRecords, islands);
	whichIsland = 0;
	whichPoint = 0;


	/**** Set up GLUT and start interaction ****/

	glutInit(&argc, argv);

	// Window 1: Manifold maps
	initManifoldMapWindow();

	// Window 2: Recolored images
	initImageWindow();

	glutMainLoop();

	FreeImage_DeInitialise();
	return 0;
}




