import sys
from typing import Generator
import regex as re


RANGE_0_59 = r"0*[1-5]?\d"  # range(0, 60)
ZERO_MM_SS =        \
    r"0+"   r"\s*"  \
    r"'"    r"\s*"  \
    r"0+"   r"\s*"  \
    r"\""   r"\s*"
ANY_MM_SS =                     \
    fr"{RANGE_0_59}"    r"\s*"  \
    r"'"                r"\s*"  \
    fr"{RANGE_0_59}"    r"\s*"  \
    r"\""               r"\s*"

ZERO_DDDD = r"0*"   r"\s*"
ANY_DDDD = r"\d*"   r"\s*"

ZERO_END = fr"(\.{ZERO_DDDD}|\s*D\s*{ZERO_MM_SS})"
ANY_END = fr"(\.{ANY_DDDD}|\s*D\s*{ANY_MM_SS})"

RANGE_0_89 = r"[1-8]?\d"
RANGE_90_90 = r"90"

RANGE_00_79 = r"[0-7]\d"
RANGE_0_99 = r"[1-9]?\d"
RANGE_0_179 = fr"(1{RANGE_00_79}|{RANGE_0_99})"
RANGE_180_180 = r"180"

LATITUDE = r"\s*" r"0*"             \
    fr"({RANGE_0_89}{ANY_END}|"     \
    fr"{RANGE_90_90}{ZERO_END})"
LONGITUDE = r"\s*" r"0*"            \
    fr"({RANGE_0_179}{ANY_END}|"    \
    fr"{RANGE_180_180}{ZERO_END})"

PATTERN_GEOGR_COORDS = r"\s*"   \
    fr"([SN]{LATITUDE}|"        \
    fr"[EW]{LONGITUDE}|)"       \
    r"\s*"


def lexems_iter(
    source_code: str
) -> Generator[tuple[bool, str, int, int, str], None, None]:

    pattern = re.compile(PATTERN_GEOGR_COORDS, re.UNICODE)

    for line_index, file_line in enumerate(source_code.split('\n'), start=1):
        last_lexem_end = 0

        for next_iter in pattern.finditer(file_line):
            lexem_col_from, lexem_col_to = next_iter.span()
            if lexem_col_from == lexem_col_to:
                continue
            if last_lexem_end != lexem_col_from:
                yield True, "", line_index, last_lexem_end, ""

            yield False, "COORDS", line_index, lexem_col_from, next_iter[0]
            last_lexem_end = lexem_col_to

        if last_lexem_end != len(file_line):
            yield True, "", line_index, last_lexem_end, ""


with open(sys.argv[1], mode="r", encoding="utf8") as input_file:

    for (
        lexem_error,
        lexem_type,
        lexem_line,
        lexem_col,
        lexem_str
    ) in lexems_iter(input_file.read()):
        if lexem_error:
            print(f"syntax error ({lexem_line}, {lexem_col})")
        else:
            print(f"{lexem_type} ({lexem_line}, {lexem_col}):",
                  repr(lexem_str))
