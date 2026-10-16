import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct Bench0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_0() {
    ++executed; assert_equal(value + 0, 1);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_3() {
    ++executed; assert_equal(value + 3, 4);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_6() {
    ++executed; assert_equal(value + 6, 7);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_9() {
    ++executed; assert_equal(value + 9, 10);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_12() {
    ++executed; assert_equal(value + 12, 13);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_15() {
    ++executed; assert_equal(value + 15, 16);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_18() {
    ++executed; assert_equal(value + 18, 19);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_21() {
    ++executed; assert_equal(value + 21, 22);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_24() {
    ++executed; assert_equal(value + 24, 25);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_27() {
    ++executed; assert_equal(value + 27, 28);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_30() {
    ++executed; assert_equal(value + 30, 31);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_33() {
    ++executed; assert_equal(value + 33, 34);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_36() {
    ++executed; assert_equal(value + 36, 37);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_39() {
    ++executed; assert_equal(value + 39, 40);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_42() {
    ++executed; assert_equal(value + 42, 43);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_45() {
    ++executed; assert_equal(value + 45, 46);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_48() {
    ++executed; assert_equal(value + 48, 49);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_51() {
    ++executed; assert_equal(value + 51, 52);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_54() {
    ++executed; assert_equal(value + 54, 55);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_57() {
    ++executed; assert_equal(value + 57, 58);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_60() {
    ++executed; assert_equal(value + 60, 61);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_63() {
    ++executed; assert_equal(value + 63, 64);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_66() {
    ++executed; assert_equal(value + 66, 67);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_69() {
    ++executed; assert_equal(value + 69, 70);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_72() {
    ++executed; assert_equal(value + 72, 73);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_75() {
    ++executed; assert_equal(value + 75, 76);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_78() {
    ++executed; assert_equal(value + 78, 79);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_81() {
    ++executed; assert_equal(value + 81, 82);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_84() {
    ++executed; assert_equal(value + 84, 85);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_87() {
    ++executed; assert_equal(value + 87, 88);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_90() {
    ++executed; assert_equal(value + 90, 91);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_93() {
    ++executed; assert_equal(value + 93, 94);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_96() {
    ++executed; assert_equal(value + 96, 97);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_99() {
    ++executed; assert_equal(value + 99, 100);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_102() {
    ++executed; assert_equal(value + 102, 103);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_105() {
    ++executed; assert_equal(value + 105, 106);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_108() {
    ++executed; assert_equal(value + 108, 109);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_111() {
    ++executed; assert_equal(value + 111, 112);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_114() {
    ++executed; assert_equal(value + 114, 115);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_117() {
    ++executed; assert_equal(value + 117, 118);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_120() {
    ++executed; assert_equal(value + 120, 121);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_123() {
    ++executed; assert_equal(value + 123, 124);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_126() {
    ++executed; assert_equal(value + 126, 127);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_129() {
    ++executed; assert_equal(value + 129, 130);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_132() {
    ++executed; assert_equal(value + 132, 133);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_135() {
    ++executed; assert_equal(value + 135, 136);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_138() {
    ++executed; assert_equal(value + 138, 139);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_141() {
    ++executed; assert_equal(value + 141, 142);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_144() {
    ++executed; assert_equal(value + 144, 145);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_147() {
    ++executed; assert_equal(value + 147, 148);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_150() {
    ++executed; assert_equal(value + 150, 151);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_153() {
    ++executed; assert_equal(value + 153, 154);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_156() {
    ++executed; assert_equal(value + 156, 157);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_159() {
    ++executed; assert_equal(value + 159, 160);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_162() {
    ++executed; assert_equal(value + 162, 163);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_165() {
    ++executed; assert_equal(value + 165, 166);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_168() {
    ++executed; assert_equal(value + 168, 169);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_171() {
    ++executed; assert_equal(value + 171, 172);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_174() {
    ++executed; assert_equal(value + 174, 175);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_177() {
    ++executed; assert_equal(value + 177, 178);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_180() {
    ++executed; assert_equal(value + 180, 181);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_183() {
    ++executed; assert_equal(value + 183, 184);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_186() {
    ++executed; assert_equal(value + 186, 187);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_189() {
    ++executed; assert_equal(value + 189, 190);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_192() {
    ++executed; assert_equal(value + 192, 193);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_195() {
    ++executed; assert_equal(value + 195, 196);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_198() {
    ++executed; assert_equal(value + 198, 199);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_201() {
    ++executed; assert_equal(value + 201, 202);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_204() {
    ++executed; assert_equal(value + 204, 205);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_207() {
    ++executed; assert_equal(value + 207, 208);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_210() {
    ++executed; assert_equal(value + 210, 211);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_213() {
    ++executed; assert_equal(value + 213, 214);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_216() {
    ++executed; assert_equal(value + 216, 217);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_219() {
    ++executed; assert_equal(value + 219, 220);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_222() {
    ++executed; assert_equal(value + 222, 223);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_225() {
    ++executed; assert_equal(value + 225, 226);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_228() {
    ++executed; assert_equal(value + 228, 229);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_231() {
    ++executed; assert_equal(value + 231, 232);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_234() {
    ++executed; assert_equal(value + 234, 235);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_237() {
    ++executed; assert_equal(value + 237, 238);
    std::cout << std::string(1024 * 1024, 'x');
  }
};
struct Bench1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_1() {
    ++executed; assert_equal(value + 1, 2);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_4() {
    ++executed; assert_equal(value + 4, 5);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_7() {
    ++executed; assert_equal(value + 7, 8);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_10() {
    ++executed; assert_equal(value + 10, 11);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_13() {
    ++executed; assert_equal(value + 13, 14);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_16() {
    ++executed; assert_equal(value + 16, 17);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_19() {
    ++executed; assert_equal(value + 19, 20);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_22() {
    ++executed; assert_equal(value + 22, 23);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_25() {
    ++executed; assert_equal(value + 25, 26);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_28() {
    ++executed; assert_equal(value + 28, 29);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_31() {
    ++executed; assert_equal(value + 31, 32);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_34() {
    ++executed; assert_equal(value + 34, 35);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_37() {
    ++executed; assert_equal(value + 37, 38);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_40() {
    ++executed; assert_equal(value + 40, 41);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_43() {
    ++executed; assert_equal(value + 43, 44);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_46() {
    ++executed; assert_equal(value + 46, 47);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_49() {
    ++executed; assert_equal(value + 49, 50);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_52() {
    ++executed; assert_equal(value + 52, 53);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_55() {
    ++executed; assert_equal(value + 55, 56);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_58() {
    ++executed; assert_equal(value + 58, 59);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_61() {
    ++executed; assert_equal(value + 61, 62);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_64() {
    ++executed; assert_equal(value + 64, 65);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_67() {
    ++executed; assert_equal(value + 67, 68);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_70() {
    ++executed; assert_equal(value + 70, 71);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_73() {
    ++executed; assert_equal(value + 73, 74);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_76() {
    ++executed; assert_equal(value + 76, 77);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_79() {
    ++executed; assert_equal(value + 79, 80);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_82() {
    ++executed; assert_equal(value + 82, 83);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_85() {
    ++executed; assert_equal(value + 85, 86);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_88() {
    ++executed; assert_equal(value + 88, 89);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_91() {
    ++executed; assert_equal(value + 91, 92);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_94() {
    ++executed; assert_equal(value + 94, 95);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_97() {
    ++executed; assert_equal(value + 97, 98);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_100() {
    ++executed; assert_equal(value + 100, 101);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_103() {
    ++executed; assert_equal(value + 103, 104);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_106() {
    ++executed; assert_equal(value + 106, 107);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_109() {
    ++executed; assert_equal(value + 109, 110);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_112() {
    ++executed; assert_equal(value + 112, 113);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_115() {
    ++executed; assert_equal(value + 115, 116);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_118() {
    ++executed; assert_equal(value + 118, 119);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_121() {
    ++executed; assert_equal(value + 121, 122);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_124() {
    ++executed; assert_equal(value + 124, 125);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_127() {
    ++executed; assert_equal(value + 127, 128);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_130() {
    ++executed; assert_equal(value + 130, 131);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_133() {
    ++executed; assert_equal(value + 133, 134);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_136() {
    ++executed; assert_equal(value + 136, 137);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_139() {
    ++executed; assert_equal(value + 139, 140);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_142() {
    ++executed; assert_equal(value + 142, 143);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_145() {
    ++executed; assert_equal(value + 145, 146);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_148() {
    ++executed; assert_equal(value + 148, 149);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_151() {
    ++executed; assert_equal(value + 151, 152);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_154() {
    ++executed; assert_equal(value + 154, 155);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_157() {
    ++executed; assert_equal(value + 157, 158);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_160() {
    ++executed; assert_equal(value + 160, 161);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_163() {
    ++executed; assert_equal(value + 163, 164);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_166() {
    ++executed; assert_equal(value + 166, 167);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_169() {
    ++executed; assert_equal(value + 169, 170);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_172() {
    ++executed; assert_equal(value + 172, 173);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_175() {
    ++executed; assert_equal(value + 175, 176);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_178() {
    ++executed; assert_equal(value + 178, 179);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_181() {
    ++executed; assert_equal(value + 181, 182);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_184() {
    ++executed; assert_equal(value + 184, 185);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_187() {
    ++executed; assert_equal(value + 187, 188);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_190() {
    ++executed; assert_equal(value + 190, 191);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_193() {
    ++executed; assert_equal(value + 193, 194);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_196() {
    ++executed; assert_equal(value + 196, 197);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_199() {
    ++executed; assert_equal(value + 199, 200);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_202() {
    ++executed; assert_equal(value + 202, 203);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_205() {
    ++executed; assert_equal(value + 205, 206);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_208() {
    ++executed; assert_equal(value + 208, 209);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_211() {
    ++executed; assert_equal(value + 211, 212);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_214() {
    ++executed; assert_equal(value + 214, 215);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_217() {
    ++executed; assert_equal(value + 217, 218);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_220() {
    ++executed; assert_equal(value + 220, 221);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_223() {
    ++executed; assert_equal(value + 223, 224);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_226() {
    ++executed; assert_equal(value + 226, 227);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_229() {
    ++executed; assert_equal(value + 229, 230);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_232() {
    ++executed; assert_equal(value + 232, 233);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_235() {
    ++executed; assert_equal(value + 235, 236);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_238() {
    ++executed; assert_equal(value + 238, 239);
    std::cout << std::string(1024 * 1024, 'x');
  }
};
struct Bench2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_2() {
    ++executed; assert_equal(value + 2, 3);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_5() {
    ++executed; assert_equal(value + 5, 6);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_8() {
    ++executed; assert_equal(value + 8, 9);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_11() {
    ++executed; assert_equal(value + 11, 12);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_14() {
    ++executed; assert_equal(value + 14, 15);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_17() {
    ++executed; assert_equal(value + 17, 18);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_20() {
    ++executed; assert_equal(value + 20, 21);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_23() {
    ++executed; assert_equal(value + 23, 24);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_26() {
    ++executed; assert_equal(value + 26, 27);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_29() {
    ++executed; assert_equal(value + 29, 30);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_32() {
    ++executed; assert_equal(value + 32, 33);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_35() {
    ++executed; assert_equal(value + 35, 36);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_38() {
    ++executed; assert_equal(value + 38, 39);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_41() {
    ++executed; assert_equal(value + 41, 42);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_44() {
    ++executed; assert_equal(value + 44, 45);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_47() {
    ++executed; assert_equal(value + 47, 48);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_50() {
    ++executed; assert_equal(value + 50, 51);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_53() {
    ++executed; assert_equal(value + 53, 54);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_56() {
    ++executed; assert_equal(value + 56, 57);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_59() {
    ++executed; assert_equal(value + 59, 60);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_62() {
    ++executed; assert_equal(value + 62, 63);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_65() {
    ++executed; assert_equal(value + 65, 66);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_68() {
    ++executed; assert_equal(value + 68, 69);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_71() {
    ++executed; assert_equal(value + 71, 72);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_74() {
    ++executed; assert_equal(value + 74, 75);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_77() {
    ++executed; assert_equal(value + 77, 78);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_80() {
    ++executed; assert_equal(value + 80, 81);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_83() {
    ++executed; assert_equal(value + 83, 84);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_86() {
    ++executed; assert_equal(value + 86, 87);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_89() {
    ++executed; assert_equal(value + 89, 90);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_92() {
    ++executed; assert_equal(value + 92, 93);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_95() {
    ++executed; assert_equal(value + 95, 96);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_98() {
    ++executed; assert_equal(value + 98, 99);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_101() {
    ++executed; assert_equal(value + 101, 102);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_104() {
    ++executed; assert_equal(value + 104, 105);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_107() {
    ++executed; assert_equal(value + 107, 108);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_110() {
    ++executed; assert_equal(value + 110, 111);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_113() {
    ++executed; assert_equal(value + 113, 114);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_116() {
    ++executed; assert_equal(value + 116, 117);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_119() {
    ++executed; assert_equal(value + 119, 120);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_122() {
    ++executed; assert_equal(value + 122, 123);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_125() {
    ++executed; assert_equal(value + 125, 126);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_128() {
    ++executed; assert_equal(value + 128, 129);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_131() {
    ++executed; assert_equal(value + 131, 132);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_134() {
    ++executed; assert_equal(value + 134, 135);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_137() {
    ++executed; assert_equal(value + 137, 138);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_140() {
    ++executed; assert_equal(value + 140, 141);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_143() {
    ++executed; assert_equal(value + 143, 144);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_146() {
    ++executed; assert_equal(value + 146, 147);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_149() {
    ++executed; assert_equal(value + 149, 150);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_152() {
    ++executed; assert_equal(value + 152, 153);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_155() {
    ++executed; assert_equal(value + 155, 156);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_158() {
    ++executed; assert_equal(value + 158, 159);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_161() {
    ++executed; assert_equal(value + 161, 162);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_164() {
    ++executed; assert_equal(value + 164, 165);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_167() {
    ++executed; assert_equal(value + 167, 168);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_170() {
    ++executed; assert_equal(value + 170, 171);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_173() {
    ++executed; assert_equal(value + 173, 174);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_176() {
    ++executed; assert_equal(value + 176, 177);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_179() {
    ++executed; assert_equal(value + 179, 180);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_182() {
    ++executed; assert_equal(value + 182, 183);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_185() {
    ++executed; assert_equal(value + 185, 186);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_188() {
    ++executed; assert_equal(value + 188, 189);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_191() {
    ++executed; assert_equal(value + 191, 192);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_194() {
    ++executed; assert_equal(value + 194, 195);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_197() {
    ++executed; assert_equal(value + 197, 198);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_200() {
    ++executed; assert_equal(value + 200, 201);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_203() {
    ++executed; assert_equal(value + 203, 204);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_206() {
    ++executed; assert_equal(value + 206, 207);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_209() {
    ++executed; assert_equal(value + 209, 210);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_212() {
    ++executed; assert_equal(value + 212, 213);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_215() {
    ++executed; assert_equal(value + 215, 216);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_218() {
    ++executed; assert_equal(value + 218, 219);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_221() {
    ++executed; assert_equal(value + 221, 222);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_224() {
    ++executed; assert_equal(value + 224, 225);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_227() {
    ++executed; assert_equal(value + 227, 228);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_230() {
    ++executed; assert_equal(value + 230, 231);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_233() {
    ++executed; assert_equal(value + 233, 234);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_236() {
    ++executed; assert_equal(value + 236, 237);
    std::cout << std::string(1024 * 1024, 'x');
  }
  [[=test{}]] void case_239() {
    ++executed; assert_equal(value + 239, 240);
    std::cout << std::string(1024 * 1024, 'x');
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<Bench0>(argc, argv);
  status |= run_tests<Bench1>(argc, argv);
  status |= run_tests<Bench2>(argc, argv);
  return status || executed != 240;
}
