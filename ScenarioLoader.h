/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * scenarioLoader.h
 * hog
 *
 * Created by Renee Jansen on 5/2/2006
 * Cleaned up by Marzo Sette Torres Junior <marzojr@dcc.ufmg.br>
 *
 */

#ifndef SCENARIOLOADER_H
#define SCENARIOLOADER_H

#include <vector>
#include <cstring>
#include <string>
using std::string;

static const int kNoScaling = -1;

/**
 * Experiments stored by the ScenarioLoader class.
 */
class ScenarioLoader;

class Experiment {
public:
	Experiment(int sx,int sy,int gx,int gy,int b, double d, string m)
		: startx(sx), starty(sy), goalx(gx), goaly(gy), scaleX(kNoScaling),
		  scaleY(kNoScaling), bucket(b), distance(d), map(m) {
	}
	Experiment(int sx,int sy,int gx,int gy,int sizeX, int sizeY,int b, double d,
	           string m)
		: startx(sx), starty(sy), goalx(gx), goaly(gy), scaleX(sizeX),
		  scaleY(sizeY), bucket(b), distance(d), map(m) {
	}
	int GetStartX() const              {	return startx;	}
	int GetStartY() const              {	return starty;	}
	int GetGoalX() const               {	return goalx;	}
	int GetGoalY() const               {	return goaly;	}
	int GetBucket() const              {	return bucket;	}
	double GetDistance() const         {	return distance;	}
	void GetMapName(char* mymap) const {	strcpy(mymap,map.c_str());	}
	string const &GetMapName() const   {	return map;	}
	int GetXScale() const              {	return scaleX;	}
	int GetYScale() const              {	return scaleY;	}
	
private:
	friend class ScenarioLoader;
	int startx, starty, goalx, goaly;
	int scaleX;
	int scaleY;
	int bucket;
	double distance;
	string map;
};

/** A class which loads and stores scenarios from files.
 * Versions currently handled: 0.0 and 1.0 (includes scale).
 */

class ScenarioLoader {
public:
	ScenarioLoader()                    {	scenName[0] = 0;	}
	ScenarioLoader(const char *);
	void Save(const char *) const;
	int GetNumExperiments() const       {	return experiments.size();	}
	char const *GetScenarioName() const {	return scenName;	}
	Experiment const &GetNthExperiment(int which) const {
		return experiments[which];
	}
	void AddExperiment(Experiment const &which) {
		experiments.push_back(which);
	}
private:
	char scenName[1024];
	std::vector<Experiment> experiments;
};

#endif
