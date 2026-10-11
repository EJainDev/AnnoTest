"""Extend Pygments' C++ lexer for C++26 annotation expressions."""
from pygments.lexer import bygroups, include, inherit
from pygments.lexers import CppLexer
from pygments.token import Name, Operator, Punctuation


class AnnoTestCppLexer(CppLexer):
    tokens = {
        state: [(r'(\[\[)(\s*=\s*)([\w]+)',
                 bygroups(Punctuation, Operator, Name.Decorator), 'annotation'), inherit]
        for state in ('statements', 'classname', 'enumname')
    }
    tokens['annotation'] = [(r'\]\]', Punctuation, '#pop'),
                            (r'[{}]', Punctuation), include('whitespace'), include('statements')]
