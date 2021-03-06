//
// Created by Cristobal M on 10/3/17.
//

#include <utility>
#include <cmath>
#include <random>
#include <unordered_map>
#include "commontypes.h"
#include "SplitHeuristic.h"
#include "IOControl.h"
#include "FilenameGenerator.h"

std::pair<int, int> SplitHeuristic::mostDistantPair(vRect &vrect) {
  float minX, minY, minX1, minY1;
  minX = minY = minX1 = minY1 = std::numeric_limits<float>::infinity();
  float maxX, maxY, maxX2, maxY2;
  maxX = maxY = maxX2 = maxY2 = -std::numeric_limits<float>::infinity();
  int rightmostx1, leftmostx2, uppermosty1, lowermosty2;
  rightmostx1 = leftmostx2 = uppermosty1 = lowermosty2 = -1;
  for(int i = 0; i < vrect.size(); i++){
    Rectangle &rect = vrect[i];
    if(maxX < rect.x1){
      maxX = rect.x1;
      rightmostx1 = i;
    }
    if(minX > rect.x2){
      minX = rect.x2;
      leftmostx2 = i;
    }
    if(maxY < rect.y1){
      maxY = rect.y1;
      uppermosty1 = i;
    }
    if(minY > rect.y2){
      minY = rect.y2;
      lowermosty2 = i;
    }
    if(minX1 > rect.x1){
      minX1 = rect.x1;
    }
    if(maxX2 < rect.x2){
      maxX2 = rect.x2;
    }
    if(minY1 > rect.y1){
      minY1 = rect.y1;
    }
    if(maxY2 < rect.y2){
      maxY2 = rect.y2;
    }
  }
  float xDimSeparationNorm = std::abs(maxX - minX)/(maxX2 - minX1);
  float yDimSeparationNorm = std::abs(maxY - minY)/(maxY2 - minY1);
  int lesserRect, mostRect;
  if(xDimSeparationNorm >= yDimSeparationNorm){
    lesserRect = leftmostx2;
    mostRect = rightmostx1;
  }
  else{
    lesserRect = lowermosty2;
    mostRect = uppermosty1;
  }

  return std::make_pair(lesserRect, mostRect);
}



void SplitHeuristic::splitNode(std::shared_ptr<RTree> rtree, std::string controllerPrefix, CachingRTree &Cached) {
  vRect &vrect = rtree->node;
  if(vrect.size() < DEFAULT_MAX_NODE_SIZE + 1){
    return;
  }
  splittedNode splitted = split(vrect);

  int parentFilenameIndex = rtree->parentFilenameIndex;
  int parentRectangleIndex = rtree->parentRectangleIndex;

  splitted.leftParent.address = FilenameGenerator::generateNewIndex();
  splitted.rightParent.address = FilenameGenerator::generateNewIndex();


  std::shared_ptr<RTree> leftRtree(new RTree(splitted.left, splitted.leftParent.address, rtree->leaf, rtree->parentFilenameIndex, rtree->parentRectangleIndex));
  std::shared_ptr<RTree> rightRtree(new RTree(splitted.right, splitted.rightParent.address, rtree->leaf, rtree->parentFilenameIndex, rtree->parentRectangleIndex));


  if(parentFilenameIndex > -1){
    rtree = IOControl::getRTree(parentFilenameIndex, controllerPrefix, Cached);
    rtree->node[parentRectangleIndex] = splitted.leftParent;
    rtree->node.push_back(splitted.rightParent);
    leftRtree->setParentRectangleIndex(parentRectangleIndex);
    rightRtree->setParentRectangleIndex((int)rtree->node.size() - 1);

  }
  else{
    rtree->node.push_back(splitted.leftParent);
    rtree->node.push_back(splitted.rightParent);
    leftRtree->setParentRectangleIndex(0);
    rightRtree->setParentRectangleIndex(1);
  }

  leftRtree->setParentFilenameIndex(rtree->inputFilenameIndex);
  rightRtree->setParentFilenameIndex(rtree->inputFilenameIndex);

    rtree->leaf = false;
  IOControl::saveRTree(leftRtree, splitted.leftParent.address, controllerPrefix, Cached);
  IOControl::saveRTree(rightRtree, splitted.rightParent.address, controllerPrefix, Cached);


  IOControl::saveRTree(leftRtree, leftRtree->getInputFilenameIndex(), controllerPrefix, Cached);
  IOControl::saveRTree(rightRtree, rightRtree->getInputFilenameIndex(), controllerPrefix, Cached);
  IOControl::saveRTree(rtree, rtree->getInputFilenameIndex(), controllerPrefix, Cached);


  splitNode(rtree, controllerPrefix, Cached);

}




std::vector<long> SplitHeuristic::fisherYatesVariation(long result_size, long choice_set_size) {
  std::vector<long> result;
  std::unordered_map<long, long> state;
  std::srand(time(0));

  for(long i = 0; i < result_size; i++){
    long random_number = (std::rand() % (choice_set_size - i)) + i;
    long whichRand = random_number;
    long whichI = i;
    if(state.find(random_number) != state.end()){
      whichRand = state[random_number];
    }
    if(state.find(i) != state.end()){
      whichI = state[i];
    }
    state[i] = whichRand;
    state[random_number] = whichI;
  }

  for(int i = 0; i < result_size; i++){
    result.push_back(state[i]+1);
  }
  return result;
}


