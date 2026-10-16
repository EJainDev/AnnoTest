import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct bench_0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(0), parameters(3), parameters(6), parameters(9), parameters(12), parameters(15), parameters(18), parameters(21), parameters(24), parameters(27), parameters(30), parameters(33), parameters(36), parameters(39), parameters(42), parameters(45), parameters(48), parameters(51), parameters(54), parameters(57), parameters(60), parameters(63), parameters(66), parameters(69), parameters(72), parameters(75), parameters(78), parameters(81), parameters(84), parameters(87), parameters(90), parameters(93), parameters(96), parameters(99), parameters(102), parameters(105), parameters(108), parameters(111), parameters(114), parameters(117), parameters(120), parameters(123), parameters(126), parameters(129), parameters(132), parameters(135), parameters(138), parameters(141), parameters(144), parameters(147), parameters(150), parameters(153), parameters(156), parameters(159), parameters(162), parameters(165), parameters(168), parameters(171), parameters(174), parameters(177), parameters(180), parameters(183), parameters(186), parameters(189), parameters(192), parameters(195), parameters(198), parameters(201), parameters(204), parameters(207), parameters(210), parameters(213), parameters(216), parameters(219), parameters(222), parameters(225), parameters(228), parameters(231), parameters(234), parameters(237)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(1), parameters(4), parameters(7), parameters(10), parameters(13), parameters(16), parameters(19), parameters(22), parameters(25), parameters(28), parameters(31), parameters(34), parameters(37), parameters(40), parameters(43), parameters(46), parameters(49), parameters(52), parameters(55), parameters(58), parameters(61), parameters(64), parameters(67), parameters(70), parameters(73), parameters(76), parameters(79), parameters(82), parameters(85), parameters(88), parameters(91), parameters(94), parameters(97), parameters(100), parameters(103), parameters(106), parameters(109), parameters(112), parameters(115), parameters(118), parameters(121), parameters(124), parameters(127), parameters(130), parameters(133), parameters(136), parameters(139), parameters(142), parameters(145), parameters(148), parameters(151), parameters(154), parameters(157), parameters(160), parameters(163), parameters(166), parameters(169), parameters(172), parameters(175), parameters(178), parameters(181), parameters(184), parameters(187), parameters(190), parameters(193), parameters(196), parameters(199), parameters(202), parameters(205), parameters(208), parameters(211), parameters(214), parameters(217), parameters(220), parameters(223), parameters(226), parameters(229), parameters(232), parameters(235), parameters(238)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(2), parameters(5), parameters(8), parameters(11), parameters(14), parameters(17), parameters(20), parameters(23), parameters(26), parameters(29), parameters(32), parameters(35), parameters(38), parameters(41), parameters(44), parameters(47), parameters(50), parameters(53), parameters(56), parameters(59), parameters(62), parameters(65), parameters(68), parameters(71), parameters(74), parameters(77), parameters(80), parameters(83), parameters(86), parameters(89), parameters(92), parameters(95), parameters(98), parameters(101), parameters(104), parameters(107), parameters(110), parameters(113), parameters(116), parameters(119), parameters(122), parameters(125), parameters(128), parameters(131), parameters(134), parameters(137), parameters(140), parameters(143), parameters(146), parameters(149), parameters(152), parameters(155), parameters(158), parameters(161), parameters(164), parameters(167), parameters(170), parameters(173), parameters(176), parameters(179), parameters(182), parameters(185), parameters(188), parameters(191), parameters(194), parameters(197), parameters(200), parameters(203), parameters(206), parameters(209), parameters(212), parameters(215), parameters(218), parameters(221), parameters(224), parameters(227), parameters(230), parameters(233), parameters(236), parameters(239)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<bench_0>(argc, argv);
  status |= run_tests<bench_1>(argc, argv);
  status |= run_tests<bench_2>(argc, argv);
  return status || executed != 240;
}
