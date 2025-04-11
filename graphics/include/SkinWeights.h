#ifndef SkinWeights_INCLUDED
#define SkinWeights_INCLUDED

#include <vector>
class SkinWeights {
public:
	SkinWeights();
	std::vector<int> jointIDs;
	std::vector<float> weights;
};
#endif