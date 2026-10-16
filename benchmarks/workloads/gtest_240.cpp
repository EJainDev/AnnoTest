#include <gtest/gtest.h>
#include <iostream>
#include <string>
static int executed = 0;
class Bench0 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench0, Case0) {
  ++executed; ASSERT_EQ(value + 0, 1);
}
TEST_F(Bench0, Case3) {
  ++executed; ASSERT_EQ(value + 3, 4);
}
TEST_F(Bench0, Case6) {
  ++executed; ASSERT_EQ(value + 6, 7);
}
TEST_F(Bench0, Case9) {
  ++executed; ASSERT_EQ(value + 9, 10);
}
TEST_F(Bench0, Case12) {
  ++executed; ASSERT_EQ(value + 12, 13);
}
TEST_F(Bench0, Case15) {
  ++executed; ASSERT_EQ(value + 15, 16);
}
TEST_F(Bench0, Case18) {
  ++executed; ASSERT_EQ(value + 18, 19);
}
TEST_F(Bench0, Case21) {
  ++executed; ASSERT_EQ(value + 21, 22);
}
TEST_F(Bench0, Case24) {
  ++executed; ASSERT_EQ(value + 24, 25);
}
TEST_F(Bench0, Case27) {
  ++executed; ASSERT_EQ(value + 27, 28);
}
TEST_F(Bench0, Case30) {
  ++executed; ASSERT_EQ(value + 30, 31);
}
TEST_F(Bench0, Case33) {
  ++executed; ASSERT_EQ(value + 33, 34);
}
TEST_F(Bench0, Case36) {
  ++executed; ASSERT_EQ(value + 36, 37);
}
TEST_F(Bench0, Case39) {
  ++executed; ASSERT_EQ(value + 39, 40);
}
TEST_F(Bench0, Case42) {
  ++executed; ASSERT_EQ(value + 42, 43);
}
TEST_F(Bench0, Case45) {
  ++executed; ASSERT_EQ(value + 45, 46);
}
TEST_F(Bench0, Case48) {
  ++executed; ASSERT_EQ(value + 48, 49);
}
TEST_F(Bench0, Case51) {
  ++executed; ASSERT_EQ(value + 51, 52);
}
TEST_F(Bench0, Case54) {
  ++executed; ASSERT_EQ(value + 54, 55);
}
TEST_F(Bench0, Case57) {
  ++executed; ASSERT_EQ(value + 57, 58);
}
TEST_F(Bench0, Case60) {
  ++executed; ASSERT_EQ(value + 60, 61);
}
TEST_F(Bench0, Case63) {
  ++executed; ASSERT_EQ(value + 63, 64);
}
TEST_F(Bench0, Case66) {
  ++executed; ASSERT_EQ(value + 66, 67);
}
TEST_F(Bench0, Case69) {
  ++executed; ASSERT_EQ(value + 69, 70);
}
TEST_F(Bench0, Case72) {
  ++executed; ASSERT_EQ(value + 72, 73);
}
TEST_F(Bench0, Case75) {
  ++executed; ASSERT_EQ(value + 75, 76);
}
TEST_F(Bench0, Case78) {
  ++executed; ASSERT_EQ(value + 78, 79);
}
TEST_F(Bench0, Case81) {
  ++executed; ASSERT_EQ(value + 81, 82);
}
TEST_F(Bench0, Case84) {
  ++executed; ASSERT_EQ(value + 84, 85);
}
TEST_F(Bench0, Case87) {
  ++executed; ASSERT_EQ(value + 87, 88);
}
TEST_F(Bench0, Case90) {
  ++executed; ASSERT_EQ(value + 90, 91);
}
TEST_F(Bench0, Case93) {
  ++executed; ASSERT_EQ(value + 93, 94);
}
TEST_F(Bench0, Case96) {
  ++executed; ASSERT_EQ(value + 96, 97);
}
TEST_F(Bench0, Case99) {
  ++executed; ASSERT_EQ(value + 99, 100);
}
TEST_F(Bench0, Case102) {
  ++executed; ASSERT_EQ(value + 102, 103);
}
TEST_F(Bench0, Case105) {
  ++executed; ASSERT_EQ(value + 105, 106);
}
TEST_F(Bench0, Case108) {
  ++executed; ASSERT_EQ(value + 108, 109);
}
TEST_F(Bench0, Case111) {
  ++executed; ASSERT_EQ(value + 111, 112);
}
TEST_F(Bench0, Case114) {
  ++executed; ASSERT_EQ(value + 114, 115);
}
TEST_F(Bench0, Case117) {
  ++executed; ASSERT_EQ(value + 117, 118);
}
TEST_F(Bench0, Case120) {
  ++executed; ASSERT_EQ(value + 120, 121);
}
TEST_F(Bench0, Case123) {
  ++executed; ASSERT_EQ(value + 123, 124);
}
TEST_F(Bench0, Case126) {
  ++executed; ASSERT_EQ(value + 126, 127);
}
TEST_F(Bench0, Case129) {
  ++executed; ASSERT_EQ(value + 129, 130);
}
TEST_F(Bench0, Case132) {
  ++executed; ASSERT_EQ(value + 132, 133);
}
TEST_F(Bench0, Case135) {
  ++executed; ASSERT_EQ(value + 135, 136);
}
TEST_F(Bench0, Case138) {
  ++executed; ASSERT_EQ(value + 138, 139);
}
TEST_F(Bench0, Case141) {
  ++executed; ASSERT_EQ(value + 141, 142);
}
TEST_F(Bench0, Case144) {
  ++executed; ASSERT_EQ(value + 144, 145);
}
TEST_F(Bench0, Case147) {
  ++executed; ASSERT_EQ(value + 147, 148);
}
TEST_F(Bench0, Case150) {
  ++executed; ASSERT_EQ(value + 150, 151);
}
TEST_F(Bench0, Case153) {
  ++executed; ASSERT_EQ(value + 153, 154);
}
TEST_F(Bench0, Case156) {
  ++executed; ASSERT_EQ(value + 156, 157);
}
TEST_F(Bench0, Case159) {
  ++executed; ASSERT_EQ(value + 159, 160);
}
TEST_F(Bench0, Case162) {
  ++executed; ASSERT_EQ(value + 162, 163);
}
TEST_F(Bench0, Case165) {
  ++executed; ASSERT_EQ(value + 165, 166);
}
TEST_F(Bench0, Case168) {
  ++executed; ASSERT_EQ(value + 168, 169);
}
TEST_F(Bench0, Case171) {
  ++executed; ASSERT_EQ(value + 171, 172);
}
TEST_F(Bench0, Case174) {
  ++executed; ASSERT_EQ(value + 174, 175);
}
TEST_F(Bench0, Case177) {
  ++executed; ASSERT_EQ(value + 177, 178);
}
TEST_F(Bench0, Case180) {
  ++executed; ASSERT_EQ(value + 180, 181);
}
TEST_F(Bench0, Case183) {
  ++executed; ASSERT_EQ(value + 183, 184);
}
TEST_F(Bench0, Case186) {
  ++executed; ASSERT_EQ(value + 186, 187);
}
TEST_F(Bench0, Case189) {
  ++executed; ASSERT_EQ(value + 189, 190);
}
TEST_F(Bench0, Case192) {
  ++executed; ASSERT_EQ(value + 192, 193);
}
TEST_F(Bench0, Case195) {
  ++executed; ASSERT_EQ(value + 195, 196);
}
TEST_F(Bench0, Case198) {
  ++executed; ASSERT_EQ(value + 198, 199);
}
TEST_F(Bench0, Case201) {
  ++executed; ASSERT_EQ(value + 201, 202);
}
TEST_F(Bench0, Case204) {
  ++executed; ASSERT_EQ(value + 204, 205);
}
TEST_F(Bench0, Case207) {
  ++executed; ASSERT_EQ(value + 207, 208);
}
TEST_F(Bench0, Case210) {
  ++executed; ASSERT_EQ(value + 210, 211);
}
TEST_F(Bench0, Case213) {
  ++executed; ASSERT_EQ(value + 213, 214);
}
TEST_F(Bench0, Case216) {
  ++executed; ASSERT_EQ(value + 216, 217);
}
TEST_F(Bench0, Case219) {
  ++executed; ASSERT_EQ(value + 219, 220);
}
TEST_F(Bench0, Case222) {
  ++executed; ASSERT_EQ(value + 222, 223);
}
TEST_F(Bench0, Case225) {
  ++executed; ASSERT_EQ(value + 225, 226);
}
TEST_F(Bench0, Case228) {
  ++executed; ASSERT_EQ(value + 228, 229);
}
TEST_F(Bench0, Case231) {
  ++executed; ASSERT_EQ(value + 231, 232);
}
TEST_F(Bench0, Case234) {
  ++executed; ASSERT_EQ(value + 234, 235);
}
TEST_F(Bench0, Case237) {
  ++executed; ASSERT_EQ(value + 237, 238);
}
class Bench1 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench1, Case1) {
  ++executed; ASSERT_EQ(value + 1, 2);
}
TEST_F(Bench1, Case4) {
  ++executed; ASSERT_EQ(value + 4, 5);
}
TEST_F(Bench1, Case7) {
  ++executed; ASSERT_EQ(value + 7, 8);
}
TEST_F(Bench1, Case10) {
  ++executed; ASSERT_EQ(value + 10, 11);
}
TEST_F(Bench1, Case13) {
  ++executed; ASSERT_EQ(value + 13, 14);
}
TEST_F(Bench1, Case16) {
  ++executed; ASSERT_EQ(value + 16, 17);
}
TEST_F(Bench1, Case19) {
  ++executed; ASSERT_EQ(value + 19, 20);
}
TEST_F(Bench1, Case22) {
  ++executed; ASSERT_EQ(value + 22, 23);
}
TEST_F(Bench1, Case25) {
  ++executed; ASSERT_EQ(value + 25, 26);
}
TEST_F(Bench1, Case28) {
  ++executed; ASSERT_EQ(value + 28, 29);
}
TEST_F(Bench1, Case31) {
  ++executed; ASSERT_EQ(value + 31, 32);
}
TEST_F(Bench1, Case34) {
  ++executed; ASSERT_EQ(value + 34, 35);
}
TEST_F(Bench1, Case37) {
  ++executed; ASSERT_EQ(value + 37, 38);
}
TEST_F(Bench1, Case40) {
  ++executed; ASSERT_EQ(value + 40, 41);
}
TEST_F(Bench1, Case43) {
  ++executed; ASSERT_EQ(value + 43, 44);
}
TEST_F(Bench1, Case46) {
  ++executed; ASSERT_EQ(value + 46, 47);
}
TEST_F(Bench1, Case49) {
  ++executed; ASSERT_EQ(value + 49, 50);
}
TEST_F(Bench1, Case52) {
  ++executed; ASSERT_EQ(value + 52, 53);
}
TEST_F(Bench1, Case55) {
  ++executed; ASSERT_EQ(value + 55, 56);
}
TEST_F(Bench1, Case58) {
  ++executed; ASSERT_EQ(value + 58, 59);
}
TEST_F(Bench1, Case61) {
  ++executed; ASSERT_EQ(value + 61, 62);
}
TEST_F(Bench1, Case64) {
  ++executed; ASSERT_EQ(value + 64, 65);
}
TEST_F(Bench1, Case67) {
  ++executed; ASSERT_EQ(value + 67, 68);
}
TEST_F(Bench1, Case70) {
  ++executed; ASSERT_EQ(value + 70, 71);
}
TEST_F(Bench1, Case73) {
  ++executed; ASSERT_EQ(value + 73, 74);
}
TEST_F(Bench1, Case76) {
  ++executed; ASSERT_EQ(value + 76, 77);
}
TEST_F(Bench1, Case79) {
  ++executed; ASSERT_EQ(value + 79, 80);
}
TEST_F(Bench1, Case82) {
  ++executed; ASSERT_EQ(value + 82, 83);
}
TEST_F(Bench1, Case85) {
  ++executed; ASSERT_EQ(value + 85, 86);
}
TEST_F(Bench1, Case88) {
  ++executed; ASSERT_EQ(value + 88, 89);
}
TEST_F(Bench1, Case91) {
  ++executed; ASSERT_EQ(value + 91, 92);
}
TEST_F(Bench1, Case94) {
  ++executed; ASSERT_EQ(value + 94, 95);
}
TEST_F(Bench1, Case97) {
  ++executed; ASSERT_EQ(value + 97, 98);
}
TEST_F(Bench1, Case100) {
  ++executed; ASSERT_EQ(value + 100, 101);
}
TEST_F(Bench1, Case103) {
  ++executed; ASSERT_EQ(value + 103, 104);
}
TEST_F(Bench1, Case106) {
  ++executed; ASSERT_EQ(value + 106, 107);
}
TEST_F(Bench1, Case109) {
  ++executed; ASSERT_EQ(value + 109, 110);
}
TEST_F(Bench1, Case112) {
  ++executed; ASSERT_EQ(value + 112, 113);
}
TEST_F(Bench1, Case115) {
  ++executed; ASSERT_EQ(value + 115, 116);
}
TEST_F(Bench1, Case118) {
  ++executed; ASSERT_EQ(value + 118, 119);
}
TEST_F(Bench1, Case121) {
  ++executed; ASSERT_EQ(value + 121, 122);
}
TEST_F(Bench1, Case124) {
  ++executed; ASSERT_EQ(value + 124, 125);
}
TEST_F(Bench1, Case127) {
  ++executed; ASSERT_EQ(value + 127, 128);
}
TEST_F(Bench1, Case130) {
  ++executed; ASSERT_EQ(value + 130, 131);
}
TEST_F(Bench1, Case133) {
  ++executed; ASSERT_EQ(value + 133, 134);
}
TEST_F(Bench1, Case136) {
  ++executed; ASSERT_EQ(value + 136, 137);
}
TEST_F(Bench1, Case139) {
  ++executed; ASSERT_EQ(value + 139, 140);
}
TEST_F(Bench1, Case142) {
  ++executed; ASSERT_EQ(value + 142, 143);
}
TEST_F(Bench1, Case145) {
  ++executed; ASSERT_EQ(value + 145, 146);
}
TEST_F(Bench1, Case148) {
  ++executed; ASSERT_EQ(value + 148, 149);
}
TEST_F(Bench1, Case151) {
  ++executed; ASSERT_EQ(value + 151, 152);
}
TEST_F(Bench1, Case154) {
  ++executed; ASSERT_EQ(value + 154, 155);
}
TEST_F(Bench1, Case157) {
  ++executed; ASSERT_EQ(value + 157, 158);
}
TEST_F(Bench1, Case160) {
  ++executed; ASSERT_EQ(value + 160, 161);
}
TEST_F(Bench1, Case163) {
  ++executed; ASSERT_EQ(value + 163, 164);
}
TEST_F(Bench1, Case166) {
  ++executed; ASSERT_EQ(value + 166, 167);
}
TEST_F(Bench1, Case169) {
  ++executed; ASSERT_EQ(value + 169, 170);
}
TEST_F(Bench1, Case172) {
  ++executed; ASSERT_EQ(value + 172, 173);
}
TEST_F(Bench1, Case175) {
  ++executed; ASSERT_EQ(value + 175, 176);
}
TEST_F(Bench1, Case178) {
  ++executed; ASSERT_EQ(value + 178, 179);
}
TEST_F(Bench1, Case181) {
  ++executed; ASSERT_EQ(value + 181, 182);
}
TEST_F(Bench1, Case184) {
  ++executed; ASSERT_EQ(value + 184, 185);
}
TEST_F(Bench1, Case187) {
  ++executed; ASSERT_EQ(value + 187, 188);
}
TEST_F(Bench1, Case190) {
  ++executed; ASSERT_EQ(value + 190, 191);
}
TEST_F(Bench1, Case193) {
  ++executed; ASSERT_EQ(value + 193, 194);
}
TEST_F(Bench1, Case196) {
  ++executed; ASSERT_EQ(value + 196, 197);
}
TEST_F(Bench1, Case199) {
  ++executed; ASSERT_EQ(value + 199, 200);
}
TEST_F(Bench1, Case202) {
  ++executed; ASSERT_EQ(value + 202, 203);
}
TEST_F(Bench1, Case205) {
  ++executed; ASSERT_EQ(value + 205, 206);
}
TEST_F(Bench1, Case208) {
  ++executed; ASSERT_EQ(value + 208, 209);
}
TEST_F(Bench1, Case211) {
  ++executed; ASSERT_EQ(value + 211, 212);
}
TEST_F(Bench1, Case214) {
  ++executed; ASSERT_EQ(value + 214, 215);
}
TEST_F(Bench1, Case217) {
  ++executed; ASSERT_EQ(value + 217, 218);
}
TEST_F(Bench1, Case220) {
  ++executed; ASSERT_EQ(value + 220, 221);
}
TEST_F(Bench1, Case223) {
  ++executed; ASSERT_EQ(value + 223, 224);
}
TEST_F(Bench1, Case226) {
  ++executed; ASSERT_EQ(value + 226, 227);
}
TEST_F(Bench1, Case229) {
  ++executed; ASSERT_EQ(value + 229, 230);
}
TEST_F(Bench1, Case232) {
  ++executed; ASSERT_EQ(value + 232, 233);
}
TEST_F(Bench1, Case235) {
  ++executed; ASSERT_EQ(value + 235, 236);
}
TEST_F(Bench1, Case238) {
  ++executed; ASSERT_EQ(value + 238, 239);
}
class Bench2 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench2, Case2) {
  ++executed; ASSERT_EQ(value + 2, 3);
}
TEST_F(Bench2, Case5) {
  ++executed; ASSERT_EQ(value + 5, 6);
}
TEST_F(Bench2, Case8) {
  ++executed; ASSERT_EQ(value + 8, 9);
}
TEST_F(Bench2, Case11) {
  ++executed; ASSERT_EQ(value + 11, 12);
}
TEST_F(Bench2, Case14) {
  ++executed; ASSERT_EQ(value + 14, 15);
}
TEST_F(Bench2, Case17) {
  ++executed; ASSERT_EQ(value + 17, 18);
}
TEST_F(Bench2, Case20) {
  ++executed; ASSERT_EQ(value + 20, 21);
}
TEST_F(Bench2, Case23) {
  ++executed; ASSERT_EQ(value + 23, 24);
}
TEST_F(Bench2, Case26) {
  ++executed; ASSERT_EQ(value + 26, 27);
}
TEST_F(Bench2, Case29) {
  ++executed; ASSERT_EQ(value + 29, 30);
}
TEST_F(Bench2, Case32) {
  ++executed; ASSERT_EQ(value + 32, 33);
}
TEST_F(Bench2, Case35) {
  ++executed; ASSERT_EQ(value + 35, 36);
}
TEST_F(Bench2, Case38) {
  ++executed; ASSERT_EQ(value + 38, 39);
}
TEST_F(Bench2, Case41) {
  ++executed; ASSERT_EQ(value + 41, 42);
}
TEST_F(Bench2, Case44) {
  ++executed; ASSERT_EQ(value + 44, 45);
}
TEST_F(Bench2, Case47) {
  ++executed; ASSERT_EQ(value + 47, 48);
}
TEST_F(Bench2, Case50) {
  ++executed; ASSERT_EQ(value + 50, 51);
}
TEST_F(Bench2, Case53) {
  ++executed; ASSERT_EQ(value + 53, 54);
}
TEST_F(Bench2, Case56) {
  ++executed; ASSERT_EQ(value + 56, 57);
}
TEST_F(Bench2, Case59) {
  ++executed; ASSERT_EQ(value + 59, 60);
}
TEST_F(Bench2, Case62) {
  ++executed; ASSERT_EQ(value + 62, 63);
}
TEST_F(Bench2, Case65) {
  ++executed; ASSERT_EQ(value + 65, 66);
}
TEST_F(Bench2, Case68) {
  ++executed; ASSERT_EQ(value + 68, 69);
}
TEST_F(Bench2, Case71) {
  ++executed; ASSERT_EQ(value + 71, 72);
}
TEST_F(Bench2, Case74) {
  ++executed; ASSERT_EQ(value + 74, 75);
}
TEST_F(Bench2, Case77) {
  ++executed; ASSERT_EQ(value + 77, 78);
}
TEST_F(Bench2, Case80) {
  ++executed; ASSERT_EQ(value + 80, 81);
}
TEST_F(Bench2, Case83) {
  ++executed; ASSERT_EQ(value + 83, 84);
}
TEST_F(Bench2, Case86) {
  ++executed; ASSERT_EQ(value + 86, 87);
}
TEST_F(Bench2, Case89) {
  ++executed; ASSERT_EQ(value + 89, 90);
}
TEST_F(Bench2, Case92) {
  ++executed; ASSERT_EQ(value + 92, 93);
}
TEST_F(Bench2, Case95) {
  ++executed; ASSERT_EQ(value + 95, 96);
}
TEST_F(Bench2, Case98) {
  ++executed; ASSERT_EQ(value + 98, 99);
}
TEST_F(Bench2, Case101) {
  ++executed; ASSERT_EQ(value + 101, 102);
}
TEST_F(Bench2, Case104) {
  ++executed; ASSERT_EQ(value + 104, 105);
}
TEST_F(Bench2, Case107) {
  ++executed; ASSERT_EQ(value + 107, 108);
}
TEST_F(Bench2, Case110) {
  ++executed; ASSERT_EQ(value + 110, 111);
}
TEST_F(Bench2, Case113) {
  ++executed; ASSERT_EQ(value + 113, 114);
}
TEST_F(Bench2, Case116) {
  ++executed; ASSERT_EQ(value + 116, 117);
}
TEST_F(Bench2, Case119) {
  ++executed; ASSERT_EQ(value + 119, 120);
}
TEST_F(Bench2, Case122) {
  ++executed; ASSERT_EQ(value + 122, 123);
}
TEST_F(Bench2, Case125) {
  ++executed; ASSERT_EQ(value + 125, 126);
}
TEST_F(Bench2, Case128) {
  ++executed; ASSERT_EQ(value + 128, 129);
}
TEST_F(Bench2, Case131) {
  ++executed; ASSERT_EQ(value + 131, 132);
}
TEST_F(Bench2, Case134) {
  ++executed; ASSERT_EQ(value + 134, 135);
}
TEST_F(Bench2, Case137) {
  ++executed; ASSERT_EQ(value + 137, 138);
}
TEST_F(Bench2, Case140) {
  ++executed; ASSERT_EQ(value + 140, 141);
}
TEST_F(Bench2, Case143) {
  ++executed; ASSERT_EQ(value + 143, 144);
}
TEST_F(Bench2, Case146) {
  ++executed; ASSERT_EQ(value + 146, 147);
}
TEST_F(Bench2, Case149) {
  ++executed; ASSERT_EQ(value + 149, 150);
}
TEST_F(Bench2, Case152) {
  ++executed; ASSERT_EQ(value + 152, 153);
}
TEST_F(Bench2, Case155) {
  ++executed; ASSERT_EQ(value + 155, 156);
}
TEST_F(Bench2, Case158) {
  ++executed; ASSERT_EQ(value + 158, 159);
}
TEST_F(Bench2, Case161) {
  ++executed; ASSERT_EQ(value + 161, 162);
}
TEST_F(Bench2, Case164) {
  ++executed; ASSERT_EQ(value + 164, 165);
}
TEST_F(Bench2, Case167) {
  ++executed; ASSERT_EQ(value + 167, 168);
}
TEST_F(Bench2, Case170) {
  ++executed; ASSERT_EQ(value + 170, 171);
}
TEST_F(Bench2, Case173) {
  ++executed; ASSERT_EQ(value + 173, 174);
}
TEST_F(Bench2, Case176) {
  ++executed; ASSERT_EQ(value + 176, 177);
}
TEST_F(Bench2, Case179) {
  ++executed; ASSERT_EQ(value + 179, 180);
}
TEST_F(Bench2, Case182) {
  ++executed; ASSERT_EQ(value + 182, 183);
}
TEST_F(Bench2, Case185) {
  ++executed; ASSERT_EQ(value + 185, 186);
}
TEST_F(Bench2, Case188) {
  ++executed; ASSERT_EQ(value + 188, 189);
}
TEST_F(Bench2, Case191) {
  ++executed; ASSERT_EQ(value + 191, 192);
}
TEST_F(Bench2, Case194) {
  ++executed; ASSERT_EQ(value + 194, 195);
}
TEST_F(Bench2, Case197) {
  ++executed; ASSERT_EQ(value + 197, 198);
}
TEST_F(Bench2, Case200) {
  ++executed; ASSERT_EQ(value + 200, 201);
}
TEST_F(Bench2, Case203) {
  ++executed; ASSERT_EQ(value + 203, 204);
}
TEST_F(Bench2, Case206) {
  ++executed; ASSERT_EQ(value + 206, 207);
}
TEST_F(Bench2, Case209) {
  ++executed; ASSERT_EQ(value + 209, 210);
}
TEST_F(Bench2, Case212) {
  ++executed; ASSERT_EQ(value + 212, 213);
}
TEST_F(Bench2, Case215) {
  ++executed; ASSERT_EQ(value + 215, 216);
}
TEST_F(Bench2, Case218) {
  ++executed; ASSERT_EQ(value + 218, 219);
}
TEST_F(Bench2, Case221) {
  ++executed; ASSERT_EQ(value + 221, 222);
}
TEST_F(Bench2, Case224) {
  ++executed; ASSERT_EQ(value + 224, 225);
}
TEST_F(Bench2, Case227) {
  ++executed; ASSERT_EQ(value + 227, 228);
}
TEST_F(Bench2, Case230) {
  ++executed; ASSERT_EQ(value + 230, 231);
}
TEST_F(Bench2, Case233) {
  ++executed; ASSERT_EQ(value + 233, 234);
}
TEST_F(Bench2, Case236) {
  ++executed; ASSERT_EQ(value + 236, 237);
}
TEST_F(Bench2, Case239) {
  ++executed; ASSERT_EQ(value + 239, 240);
}
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 240;
}
