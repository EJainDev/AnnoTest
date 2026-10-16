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
  }
  [[=test{}]] void case_3() {
    ++executed; assert_equal(value + 3, 4);
  }
  [[=test{}]] void case_6() {
    ++executed; assert_equal(value + 6, 7);
  }
  [[=test{}]] void case_9() {
    ++executed; assert_equal(value + 9, 10);
  }
  [[=test{}]] void case_12() {
    ++executed; assert_equal(value + 12, 13);
  }
  [[=test{}]] void case_15() {
    ++executed; assert_equal(value + 15, 16);
  }
  [[=test{}]] void case_18() {
    ++executed; assert_equal(value + 18, 19);
  }
  [[=test{}]] void case_21() {
    ++executed; assert_equal(value + 21, 22);
  }
  [[=test{}]] void case_24() {
    ++executed; assert_equal(value + 24, 25);
  }
  [[=test{}]] void case_27() {
    ++executed; assert_equal(value + 27, 28);
  }
  [[=test{}]] void case_30() {
    ++executed; assert_equal(value + 30, 31);
  }
  [[=test{}]] void case_33() {
    ++executed; assert_equal(value + 33, 34);
  }
  [[=test{}]] void case_36() {
    ++executed; assert_equal(value + 36, 37);
  }
  [[=test{}]] void case_39() {
    ++executed; assert_equal(value + 39, 40);
  }
  [[=test{}]] void case_42() {
    ++executed; assert_equal(value + 42, 43);
  }
  [[=test{}]] void case_45() {
    ++executed; assert_equal(value + 45, 46);
  }
  [[=test{}]] void case_48() {
    ++executed; assert_equal(value + 48, 49);
  }
  [[=test{}]] void case_51() {
    ++executed; assert_equal(value + 51, 52);
  }
  [[=test{}]] void case_54() {
    ++executed; assert_equal(value + 54, 55);
  }
  [[=test{}]] void case_57() {
    ++executed; assert_equal(value + 57, 58);
  }
  [[=test{}]] void case_60() {
    ++executed; assert_equal(value + 60, 61);
  }
  [[=test{}]] void case_63() {
    ++executed; assert_equal(value + 63, 64);
  }
  [[=test{}]] void case_66() {
    ++executed; assert_equal(value + 66, 67);
  }
  [[=test{}]] void case_69() {
    ++executed; assert_equal(value + 69, 70);
  }
  [[=test{}]] void case_72() {
    ++executed; assert_equal(value + 72, 73);
  }
  [[=test{}]] void case_75() {
    ++executed; assert_equal(value + 75, 76);
  }
  [[=test{}]] void case_78() {
    ++executed; assert_equal(value + 78, 79);
  }
  [[=test{}]] void case_81() {
    ++executed; assert_equal(value + 81, 82);
  }
  [[=test{}]] void case_84() {
    ++executed; assert_equal(value + 84, 85);
  }
  [[=test{}]] void case_87() {
    ++executed; assert_equal(value + 87, 88);
  }
  [[=test{}]] void case_90() {
    ++executed; assert_equal(value + 90, 91);
  }
  [[=test{}]] void case_93() {
    ++executed; assert_equal(value + 93, 94);
  }
  [[=test{}]] void case_96() {
    ++executed; assert_equal(value + 96, 97);
  }
  [[=test{}]] void case_99() {
    ++executed; assert_equal(value + 99, 100);
  }
  [[=test{}]] void case_102() {
    ++executed; assert_equal(value + 102, 103);
  }
  [[=test{}]] void case_105() {
    ++executed; assert_equal(value + 105, 106);
  }
  [[=test{}]] void case_108() {
    ++executed; assert_equal(value + 108, 109);
  }
  [[=test{}]] void case_111() {
    ++executed; assert_equal(value + 111, 112);
  }
  [[=test{}]] void case_114() {
    ++executed; assert_equal(value + 114, 115);
  }
  [[=test{}]] void case_117() {
    ++executed; assert_equal(value + 117, 118);
  }
  [[=test{}]] void case_120() {
    ++executed; assert_equal(value + 120, 121);
  }
  [[=test{}]] void case_123() {
    ++executed; assert_equal(value + 123, 124);
  }
  [[=test{}]] void case_126() {
    ++executed; assert_equal(value + 126, 127);
  }
  [[=test{}]] void case_129() {
    ++executed; assert_equal(value + 129, 130);
  }
  [[=test{}]] void case_132() {
    ++executed; assert_equal(value + 132, 133);
  }
  [[=test{}]] void case_135() {
    ++executed; assert_equal(value + 135, 136);
  }
  [[=test{}]] void case_138() {
    ++executed; assert_equal(value + 138, 139);
  }
  [[=test{}]] void case_141() {
    ++executed; assert_equal(value + 141, 142);
  }
  [[=test{}]] void case_144() {
    ++executed; assert_equal(value + 144, 145);
  }
  [[=test{}]] void case_147() {
    ++executed; assert_equal(value + 147, 148);
  }
  [[=test{}]] void case_150() {
    ++executed; assert_equal(value + 150, 151);
  }
  [[=test{}]] void case_153() {
    ++executed; assert_equal(value + 153, 154);
  }
  [[=test{}]] void case_156() {
    ++executed; assert_equal(value + 156, 157);
  }
  [[=test{}]] void case_159() {
    ++executed; assert_equal(value + 159, 160);
  }
  [[=test{}]] void case_162() {
    ++executed; assert_equal(value + 162, 163);
  }
  [[=test{}]] void case_165() {
    ++executed; assert_equal(value + 165, 166);
  }
  [[=test{}]] void case_168() {
    ++executed; assert_equal(value + 168, 169);
  }
  [[=test{}]] void case_171() {
    ++executed; assert_equal(value + 171, 172);
  }
  [[=test{}]] void case_174() {
    ++executed; assert_equal(value + 174, 175);
  }
  [[=test{}]] void case_177() {
    ++executed; assert_equal(value + 177, 178);
  }
  [[=test{}]] void case_180() {
    ++executed; assert_equal(value + 180, 181);
  }
  [[=test{}]] void case_183() {
    ++executed; assert_equal(value + 183, 184);
  }
  [[=test{}]] void case_186() {
    ++executed; assert_equal(value + 186, 187);
  }
  [[=test{}]] void case_189() {
    ++executed; assert_equal(value + 189, 190);
  }
  [[=test{}]] void case_192() {
    ++executed; assert_equal(value + 192, 193);
  }
  [[=test{}]] void case_195() {
    ++executed; assert_equal(value + 195, 196);
  }
  [[=test{}]] void case_198() {
    ++executed; assert_equal(value + 198, 199);
  }
  [[=test{}]] void case_201() {
    ++executed; assert_equal(value + 201, 202);
  }
  [[=test{}]] void case_204() {
    ++executed; assert_equal(value + 204, 205);
  }
  [[=test{}]] void case_207() {
    ++executed; assert_equal(value + 207, 208);
  }
  [[=test{}]] void case_210() {
    ++executed; assert_equal(value + 210, 211);
  }
  [[=test{}]] void case_213() {
    ++executed; assert_equal(value + 213, 214);
  }
  [[=test{}]] void case_216() {
    ++executed; assert_equal(value + 216, 217);
  }
  [[=test{}]] void case_219() {
    ++executed; assert_equal(value + 219, 220);
  }
  [[=test{}]] void case_222() {
    ++executed; assert_equal(value + 222, 223);
  }
  [[=test{}]] void case_225() {
    ++executed; assert_equal(value + 225, 226);
  }
  [[=test{}]] void case_228() {
    ++executed; assert_equal(value + 228, 229);
  }
  [[=test{}]] void case_231() {
    ++executed; assert_equal(value + 231, 232);
  }
  [[=test{}]] void case_234() {
    ++executed; assert_equal(value + 234, 235);
  }
  [[=test{}]] void case_237() {
    ++executed; assert_equal(value + 237, 238);
  }
};
struct Bench1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_1() {
    ++executed; assert_equal(value + 1, 2);
  }
  [[=test{}]] void case_4() {
    ++executed; assert_equal(value + 4, 5);
  }
  [[=test{}]] void case_7() {
    ++executed; assert_equal(value + 7, 8);
  }
  [[=test{}]] void case_10() {
    ++executed; assert_equal(value + 10, 11);
  }
  [[=test{}]] void case_13() {
    ++executed; assert_equal(value + 13, 14);
  }
  [[=test{}]] void case_16() {
    ++executed; assert_equal(value + 16, 17);
  }
  [[=test{}]] void case_19() {
    ++executed; assert_equal(value + 19, 20);
  }
  [[=test{}]] void case_22() {
    ++executed; assert_equal(value + 22, 23);
  }
  [[=test{}]] void case_25() {
    ++executed; assert_equal(value + 25, 26);
  }
  [[=test{}]] void case_28() {
    ++executed; assert_equal(value + 28, 29);
  }
  [[=test{}]] void case_31() {
    ++executed; assert_equal(value + 31, 32);
  }
  [[=test{}]] void case_34() {
    ++executed; assert_equal(value + 34, 35);
  }
  [[=test{}]] void case_37() {
    ++executed; assert_equal(value + 37, 38);
  }
  [[=test{}]] void case_40() {
    ++executed; assert_equal(value + 40, 41);
  }
  [[=test{}]] void case_43() {
    ++executed; assert_equal(value + 43, 44);
  }
  [[=test{}]] void case_46() {
    ++executed; assert_equal(value + 46, 47);
  }
  [[=test{}]] void case_49() {
    ++executed; assert_equal(value + 49, 50);
  }
  [[=test{}]] void case_52() {
    ++executed; assert_equal(value + 52, 53);
  }
  [[=test{}]] void case_55() {
    ++executed; assert_equal(value + 55, 56);
  }
  [[=test{}]] void case_58() {
    ++executed; assert_equal(value + 58, 59);
  }
  [[=test{}]] void case_61() {
    ++executed; assert_equal(value + 61, 62);
  }
  [[=test{}]] void case_64() {
    ++executed; assert_equal(value + 64, 65);
  }
  [[=test{}]] void case_67() {
    ++executed; assert_equal(value + 67, 68);
  }
  [[=test{}]] void case_70() {
    ++executed; assert_equal(value + 70, 71);
  }
  [[=test{}]] void case_73() {
    ++executed; assert_equal(value + 73, 74);
  }
  [[=test{}]] void case_76() {
    ++executed; assert_equal(value + 76, 77);
  }
  [[=test{}]] void case_79() {
    ++executed; assert_equal(value + 79, 80);
  }
  [[=test{}]] void case_82() {
    ++executed; assert_equal(value + 82, 83);
  }
  [[=test{}]] void case_85() {
    ++executed; assert_equal(value + 85, 86);
  }
  [[=test{}]] void case_88() {
    ++executed; assert_equal(value + 88, 89);
  }
  [[=test{}]] void case_91() {
    ++executed; assert_equal(value + 91, 92);
  }
  [[=test{}]] void case_94() {
    ++executed; assert_equal(value + 94, 95);
  }
  [[=test{}]] void case_97() {
    ++executed; assert_equal(value + 97, 98);
  }
  [[=test{}]] void case_100() {
    ++executed; assert_equal(value + 100, 101);
  }
  [[=test{}]] void case_103() {
    ++executed; assert_equal(value + 103, 104);
  }
  [[=test{}]] void case_106() {
    ++executed; assert_equal(value + 106, 107);
  }
  [[=test{}]] void case_109() {
    ++executed; assert_equal(value + 109, 110);
  }
  [[=test{}]] void case_112() {
    ++executed; assert_equal(value + 112, 113);
  }
  [[=test{}]] void case_115() {
    ++executed; assert_equal(value + 115, 116);
  }
  [[=test{}]] void case_118() {
    ++executed; assert_equal(value + 118, 119);
  }
  [[=test{}]] void case_121() {
    ++executed; assert_equal(value + 121, 122);
  }
  [[=test{}]] void case_124() {
    ++executed; assert_equal(value + 124, 125);
  }
  [[=test{}]] void case_127() {
    ++executed; assert_equal(value + 127, 128);
  }
  [[=test{}]] void case_130() {
    ++executed; assert_equal(value + 130, 131);
  }
  [[=test{}]] void case_133() {
    ++executed; assert_equal(value + 133, 134);
  }
  [[=test{}]] void case_136() {
    ++executed; assert_equal(value + 136, 137);
  }
  [[=test{}]] void case_139() {
    ++executed; assert_equal(value + 139, 140);
  }
  [[=test{}]] void case_142() {
    ++executed; assert_equal(value + 142, 143);
  }
  [[=test{}]] void case_145() {
    ++executed; assert_equal(value + 145, 146);
  }
  [[=test{}]] void case_148() {
    ++executed; assert_equal(value + 148, 149);
  }
  [[=test{}]] void case_151() {
    ++executed; assert_equal(value + 151, 152);
  }
  [[=test{}]] void case_154() {
    ++executed; assert_equal(value + 154, 155);
  }
  [[=test{}]] void case_157() {
    ++executed; assert_equal(value + 157, 158);
  }
  [[=test{}]] void case_160() {
    ++executed; assert_equal(value + 160, 161);
  }
  [[=test{}]] void case_163() {
    ++executed; assert_equal(value + 163, 164);
  }
  [[=test{}]] void case_166() {
    ++executed; assert_equal(value + 166, 167);
  }
  [[=test{}]] void case_169() {
    ++executed; assert_equal(value + 169, 170);
  }
  [[=test{}]] void case_172() {
    ++executed; assert_equal(value + 172, 173);
  }
  [[=test{}]] void case_175() {
    ++executed; assert_equal(value + 175, 176);
  }
  [[=test{}]] void case_178() {
    ++executed; assert_equal(value + 178, 179);
  }
  [[=test{}]] void case_181() {
    ++executed; assert_equal(value + 181, 182);
  }
  [[=test{}]] void case_184() {
    ++executed; assert_equal(value + 184, 185);
  }
  [[=test{}]] void case_187() {
    ++executed; assert_equal(value + 187, 188);
  }
  [[=test{}]] void case_190() {
    ++executed; assert_equal(value + 190, 191);
  }
  [[=test{}]] void case_193() {
    ++executed; assert_equal(value + 193, 194);
  }
  [[=test{}]] void case_196() {
    ++executed; assert_equal(value + 196, 197);
  }
  [[=test{}]] void case_199() {
    ++executed; assert_equal(value + 199, 200);
  }
  [[=test{}]] void case_202() {
    ++executed; assert_equal(value + 202, 203);
  }
  [[=test{}]] void case_205() {
    ++executed; assert_equal(value + 205, 206);
  }
  [[=test{}]] void case_208() {
    ++executed; assert_equal(value + 208, 209);
  }
  [[=test{}]] void case_211() {
    ++executed; assert_equal(value + 211, 212);
  }
  [[=test{}]] void case_214() {
    ++executed; assert_equal(value + 214, 215);
  }
  [[=test{}]] void case_217() {
    ++executed; assert_equal(value + 217, 218);
  }
  [[=test{}]] void case_220() {
    ++executed; assert_equal(value + 220, 221);
  }
  [[=test{}]] void case_223() {
    ++executed; assert_equal(value + 223, 224);
  }
  [[=test{}]] void case_226() {
    ++executed; assert_equal(value + 226, 227);
  }
  [[=test{}]] void case_229() {
    ++executed; assert_equal(value + 229, 230);
  }
  [[=test{}]] void case_232() {
    ++executed; assert_equal(value + 232, 233);
  }
  [[=test{}]] void case_235() {
    ++executed; assert_equal(value + 235, 236);
  }
  [[=test{}]] void case_238() {
    ++executed; assert_equal(value + 238, 239);
  }
};
struct Bench2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_2() {
    ++executed; assert_equal(value + 2, 3);
  }
  [[=test{}]] void case_5() {
    ++executed; assert_equal(value + 5, 6);
  }
  [[=test{}]] void case_8() {
    ++executed; assert_equal(value + 8, 9);
  }
  [[=test{}]] void case_11() {
    ++executed; assert_equal(value + 11, 12);
  }
  [[=test{}]] void case_14() {
    ++executed; assert_equal(value + 14, 15);
  }
  [[=test{}]] void case_17() {
    ++executed; assert_equal(value + 17, 18);
  }
  [[=test{}]] void case_20() {
    ++executed; assert_equal(value + 20, 21);
  }
  [[=test{}]] void case_23() {
    ++executed; assert_equal(value + 23, 24);
  }
  [[=test{}]] void case_26() {
    ++executed; assert_equal(value + 26, 27);
  }
  [[=test{}]] void case_29() {
    ++executed; assert_equal(value + 29, 30);
  }
  [[=test{}]] void case_32() {
    ++executed; assert_equal(value + 32, 33);
  }
  [[=test{}]] void case_35() {
    ++executed; assert_equal(value + 35, 36);
  }
  [[=test{}]] void case_38() {
    ++executed; assert_equal(value + 38, 39);
  }
  [[=test{}]] void case_41() {
    ++executed; assert_equal(value + 41, 42);
  }
  [[=test{}]] void case_44() {
    ++executed; assert_equal(value + 44, 45);
  }
  [[=test{}]] void case_47() {
    ++executed; assert_equal(value + 47, 48);
  }
  [[=test{}]] void case_50() {
    ++executed; assert_equal(value + 50, 51);
  }
  [[=test{}]] void case_53() {
    ++executed; assert_equal(value + 53, 54);
  }
  [[=test{}]] void case_56() {
    ++executed; assert_equal(value + 56, 57);
  }
  [[=test{}]] void case_59() {
    ++executed; assert_equal(value + 59, 60);
  }
  [[=test{}]] void case_62() {
    ++executed; assert_equal(value + 62, 63);
  }
  [[=test{}]] void case_65() {
    ++executed; assert_equal(value + 65, 66);
  }
  [[=test{}]] void case_68() {
    ++executed; assert_equal(value + 68, 69);
  }
  [[=test{}]] void case_71() {
    ++executed; assert_equal(value + 71, 72);
  }
  [[=test{}]] void case_74() {
    ++executed; assert_equal(value + 74, 75);
  }
  [[=test{}]] void case_77() {
    ++executed; assert_equal(value + 77, 78);
  }
  [[=test{}]] void case_80() {
    ++executed; assert_equal(value + 80, 81);
  }
  [[=test{}]] void case_83() {
    ++executed; assert_equal(value + 83, 84);
  }
  [[=test{}]] void case_86() {
    ++executed; assert_equal(value + 86, 87);
  }
  [[=test{}]] void case_89() {
    ++executed; assert_equal(value + 89, 90);
  }
  [[=test{}]] void case_92() {
    ++executed; assert_equal(value + 92, 93);
  }
  [[=test{}]] void case_95() {
    ++executed; assert_equal(value + 95, 96);
  }
  [[=test{}]] void case_98() {
    ++executed; assert_equal(value + 98, 99);
  }
  [[=test{}]] void case_101() {
    ++executed; assert_equal(value + 101, 102);
  }
  [[=test{}]] void case_104() {
    ++executed; assert_equal(value + 104, 105);
  }
  [[=test{}]] void case_107() {
    ++executed; assert_equal(value + 107, 108);
  }
  [[=test{}]] void case_110() {
    ++executed; assert_equal(value + 110, 111);
  }
  [[=test{}]] void case_113() {
    ++executed; assert_equal(value + 113, 114);
  }
  [[=test{}]] void case_116() {
    ++executed; assert_equal(value + 116, 117);
  }
  [[=test{}]] void case_119() {
    ++executed; assert_equal(value + 119, 120);
  }
  [[=test{}]] void case_122() {
    ++executed; assert_equal(value + 122, 123);
  }
  [[=test{}]] void case_125() {
    ++executed; assert_equal(value + 125, 126);
  }
  [[=test{}]] void case_128() {
    ++executed; assert_equal(value + 128, 129);
  }
  [[=test{}]] void case_131() {
    ++executed; assert_equal(value + 131, 132);
  }
  [[=test{}]] void case_134() {
    ++executed; assert_equal(value + 134, 135);
  }
  [[=test{}]] void case_137() {
    ++executed; assert_equal(value + 137, 138);
  }
  [[=test{}]] void case_140() {
    ++executed; assert_equal(value + 140, 141);
  }
  [[=test{}]] void case_143() {
    ++executed; assert_equal(value + 143, 144);
  }
  [[=test{}]] void case_146() {
    ++executed; assert_equal(value + 146, 147);
  }
  [[=test{}]] void case_149() {
    ++executed; assert_equal(value + 149, 150);
  }
  [[=test{}]] void case_152() {
    ++executed; assert_equal(value + 152, 153);
  }
  [[=test{}]] void case_155() {
    ++executed; assert_equal(value + 155, 156);
  }
  [[=test{}]] void case_158() {
    ++executed; assert_equal(value + 158, 159);
  }
  [[=test{}]] void case_161() {
    ++executed; assert_equal(value + 161, 162);
  }
  [[=test{}]] void case_164() {
    ++executed; assert_equal(value + 164, 165);
  }
  [[=test{}]] void case_167() {
    ++executed; assert_equal(value + 167, 168);
  }
  [[=test{}]] void case_170() {
    ++executed; assert_equal(value + 170, 171);
  }
  [[=test{}]] void case_173() {
    ++executed; assert_equal(value + 173, 174);
  }
  [[=test{}]] void case_176() {
    ++executed; assert_equal(value + 176, 177);
  }
  [[=test{}]] void case_179() {
    ++executed; assert_equal(value + 179, 180);
  }
  [[=test{}]] void case_182() {
    ++executed; assert_equal(value + 182, 183);
  }
  [[=test{}]] void case_185() {
    ++executed; assert_equal(value + 185, 186);
  }
  [[=test{}]] void case_188() {
    ++executed; assert_equal(value + 188, 189);
  }
  [[=test{}]] void case_191() {
    ++executed; assert_equal(value + 191, 192);
  }
  [[=test{}]] void case_194() {
    ++executed; assert_equal(value + 194, 195);
  }
  [[=test{}]] void case_197() {
    ++executed; assert_equal(value + 197, 198);
  }
  [[=test{}]] void case_200() {
    ++executed; assert_equal(value + 200, 201);
  }
  [[=test{}]] void case_203() {
    ++executed; assert_equal(value + 203, 204);
  }
  [[=test{}]] void case_206() {
    ++executed; assert_equal(value + 206, 207);
  }
  [[=test{}]] void case_209() {
    ++executed; assert_equal(value + 209, 210);
  }
  [[=test{}]] void case_212() {
    ++executed; assert_equal(value + 212, 213);
  }
  [[=test{}]] void case_215() {
    ++executed; assert_equal(value + 215, 216);
  }
  [[=test{}]] void case_218() {
    ++executed; assert_equal(value + 218, 219);
  }
  [[=test{}]] void case_221() {
    ++executed; assert_equal(value + 221, 222);
  }
  [[=test{}]] void case_224() {
    ++executed; assert_equal(value + 224, 225);
  }
  [[=test{}]] void case_227() {
    ++executed; assert_equal(value + 227, 228);
  }
  [[=test{}]] void case_230() {
    ++executed; assert_equal(value + 230, 231);
  }
  [[=test{}]] void case_233() {
    ++executed; assert_equal(value + 233, 234);
  }
  [[=test{}]] void case_236() {
    ++executed; assert_equal(value + 236, 237);
  }
  [[=test{}]] void case_239() {
    ++executed; assert_equal(value + 239, 240);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<Bench0>(argc, argv);
  status |= run_tests<Bench1>(argc, argv);
  status |= run_tests<Bench2>(argc, argv);
  return status || executed != 240;
}
