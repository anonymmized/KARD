#pragma once

#include <string>

const std::string TEXT_TO_INPUT = R"(001 | KARD sandbox renderer test.
002 | This text is intentionally long enough to be taller than the terminal viewport.
003 | Use j to scroll down and k to scroll up.
004 | Every line has a number so viewport movement is visible.
005 | Short line.
006 | This is a much longer line that should wrap when the terminal is narrow, so one logical line becomes several wrapped visual lines.
007 | The renderer should draw only visible wrapped lines.
008 | The renderer should clear old rows before drawing new content.
009 | If this line disappears correctly, clearing works.
010 | If long old text remains after scrolling, render is not clearing enough.
011 | Scroll checkpoint A.
012 | Another ordinary line.
013 | Another ordinary line.
014 | Another ordinary line.
015 | Another ordinary line.
016 | Another ordinary line.
017 | Another ordinary line.
018 | Another ordinary line.
019 | Another ordinary line.
020 | Scroll checkpoint B.
021 | The first visible line should change when firstVisible changes.
022 | The last visible line is firstVisible + outputHeight - 1.
023 | maxFirstVisible prevents scrolling below the end.
024 | clampViewport prevents negative firstVisible.
025 | Long wrapping test: abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz.
026 | Another long wrapping test: the quick brown fox jumps over the lazy dog, then walks around the terminal viewport several times.
027 | Scroll checkpoint C.
028 | Line.
029 | Line.
030 | Line.
031 | Line.
032 | Line.
033 | Line.
034 | Line.
035 | Line.
036 | Scroll checkpoint D.
037 | Line.
038 | Line.
039 | Line.
040 | Line.
041 | Line.
042 | Line.
043 | Line.
044 | Line.
045 | Scroll checkpoint E.
046 | Near the bottom.
047 | Near the bottom.
048 | Near the bottom.
049 | Near the bottom.
050 | Last line. If you can reach this with j, scrolling down works.)";
