/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the saving and loading of QTables/States
 **/

#include <stdio.h>
#include <gtest/gtest.h>
#include "Student/LBDStudent.h"
#include "QLearner/StandardQLearner.h"

namespace Primitives {

class SaveLoadTest : public testing::Test {
 protected:
  SaveLoadTest() {
    student_ = new LBDStudent();
    loaded_skill_ = new StandardQLearner("empty");

    vector<double> nearby_thresholds;
    double xy_min = 0.01;
    double z_min = 20.;
    double nearby_multiplier = 5.;

    nearby_thresholds.push_back(xy_min*nearby_multiplier);
    nearby_thresholds.push_back(xy_min*nearby_multiplier);
    nearby_thresholds.push_back(z_min*nearby_multiplier);
    nearby_thresholds.push_back(xy_min*nearby_multiplier);
    nearby_thresholds.push_back(xy_min*nearby_multiplier);
    nearby_thresholds.push_back(z_min*nearby_multiplier);

    double sampling_frequency = 1000./29.;
    skill_ = student_->LearnSkillFromFile("Primitives/Student/test.csv", 
                                          "TestSkill");
    skill_->get_q_table()->set_nearby_thresholds(nearby_thresholds);
    skill_->set_anticipated_duration(sampling_frequency
                              * skill_->get_q_table()->get_states().size());    
    
    skill_->Save("temp.shl");
    loaded_skill_->Load("temp.shl");    
  }
  
  virtual void SetUp() {

  }

  virtual ~SaveLoadTest() {
    delete student_; // Also deletes skill_
    delete loaded_skill_;
    remove("test.shl");
  }

  LBDStudent* student_;
  QLearner* skill_;
  QLearner* loaded_skill_;
};

/**
 * @test    Make sure the skill/file is there to test against
 **/
TEST_F(SaveLoadTest, SanityCheck) {
  int fail = (student_->GetSkill("TestSkill") == NULL) ? 0 : 1;
  ASSERT_NE(fail,0);
  ASSERT_TRUE(loaded_skill_->Load("temp.shl"));
}

/**
 * @test    Verify the header info saved/loaded properly
 */
TEST_F(SaveLoadTest, HeaderInfoCheck) {  
  // Check basic header info
  EXPECT_STREQ(skill_->get_name().c_str(), loaded_skill_->get_name().c_str());
  EXPECT_EQ(skill_->get_trials(), loaded_skill_->get_trials());
  EXPECT_EQ(skill_->get_anticipated_duration(),
            loaded_skill_->get_anticipated_duration());
}

/**
 * @test    Check to make sure the QTable is the same size
 **/
TEST_F(SaveLoadTest, QTableCheck) {  
  // Check Q-Table size info
  QTable *truth_table = skill_->get_q_table();
  QTable *test_table = loaded_skill_->get_q_table();
  EXPECT_EQ(truth_table->get_states().size(), test_table->get_states().size());
  EXPECT_EQ(truth_table->get_goal_states().size(),
            test_table->get_goal_states().size());
  EXPECT_EQ(truth_table->get_trained_goal_states().size(),
            test_table->get_trained_goal_states().size());
  EXPECT_EQ(truth_table->get_nearby_thresholds().size(),
            test_table->get_nearby_thresholds().size());
}

/**
 * @todo: Diff a saved skill with a loaded/saved skill
 **/

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
