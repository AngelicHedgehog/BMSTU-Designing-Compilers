import parser_edsl as pe
import sys
import re
from dataclasses import dataclass
from pprint import pprint


@dataclass
class Altrule:
    lexems : tuple[str]

@dataclass
class Rule:
    nonTerm : str
    alts : tuple[Altrule]

TERM = pe.Terminal(
    'TERMINAL',
    r'[^a-zA-Z<>\s]',
    str
)

NTERM = pe.Terminal(
    'NON_TERMINAL',
    r'[a-zA-Z][^<>\s]*',
    str
)

NRules       = pe.NonTerminal('rules')
NRule        = pe.NonTerminal('rule')
NAltrules    = pe.NonTerminal('alternate rules')
NAltrule     = pe.NonTerminal('alternate rule')

NRules |= NRule, NRules, lambda rule, rules: (rule, *rules)
NRules |= lambda: ()

NRule |= (
    '<', NTERM, '<', NAltrule, '>', NAltrules, '>',
    lambda nterm, altrule, altrules: Rule(nterm, (Altrule(altrule), *altrules))
)

NAltrules |= (
    '<', NAltrule, '>', NAltrules,
    lambda altrule, altrules: (Altrule(altrule), *altrules)
)
NAltrules |= lambda: ()

NAltrule |= TERM, NAltrule, lambda term, altrule: (term, *altrule)
NAltrule |= NTERM, NAltrule, lambda nterm, altrule: (nterm, *altrule)
NAltrule |= lambda: ()

p = pe.Parser(NRules)
assert p.is_lalr_one()

p.add_skipped_domain(r'\s')
p.add_skipped_domain(r'\'.*')

for filename in sys.argv[1:]:
    try:
        with open(filename) as f:
            tree = p.parse(f.read())
            pprint(tree)
    except pe.Error as e:
        print(f'Ошибка {e.pos}: {e.message}')
    except Exception as e:
        print(e)
