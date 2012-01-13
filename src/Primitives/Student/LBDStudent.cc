/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a student that can learn from files akin to
 * Learning By Demonstration.
 **/

#include <fstream>
#include "Student/LBDStudent.h"

namespace Primitives {

  QLearner* LBDStudent::LearnSkillFromFile(std::string filename) {
    std::ifstream training_file(filename.c_str());
    if (!training_file.is_open()) return NULL;




    training_file.close();

    return NULL;
  }

  void LBDStudent::AddTrainingToSkill(std::string primitive_id,
                                      std::string filename) {
  }
}  // namespace Primitives
