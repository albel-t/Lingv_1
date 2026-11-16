# ============================================================
# Лабораторная 2.3 — МЕТОД РЕКУРСИВНОГО СПУСКА
# Вход:  example.txt
# Матрица: 3laba_tablica_dla_progi.xlsx / Лист1  (B2:L12)
# ============================================================
import re
import openpyxl

# ---------- 1. Матрица предшествования ИЗ EXCEL ----------
wb = openpyxl.load_workbook("3laba_tablica_dla_progi.xlsx", data_only=True)
ws = wb["Лист1"]

col_names = [ws.cell(row=1, column=c).value for c in range(2, 13)]  # B..L
row_names = [ws.cell(row=r, column=1).value for r in range(2, 13)]  # 2..12

PREC = {}
for i, rn in enumerate(row_names):
    PREC[rn] = {}
    for j, cn in enumerate(col_names):
        v = ws.cell(row=i + 2, column=j + 2).value
        PREC[rn][cn] = str(v).strip() if v is not None else None

def prec(a, b):
    return PREC.get(a, {}).get(b)

# ---------- 2. Лексер ----------
TOKEN_RE = [
    ('newline', r'\n'),
    ('ид',      r'[A-Za-z_][A-Za-z0-9_]*'),
    ('конст',   r'\d+'),
    ('type',    r'\bTYPE\b'),
    ('if',      r'\bif\b'),
    ('=',       r'='),
    ('(',       r'\('),
    (')',       r'\)'),
    ('.',       r'\.'),
    (',',       r','),
    ('*',       r'\*'),
]

def lex(text):
    tokens, pos = [], 0
    while pos < len(text):
        m = re.match(r'[ \t\r]+', text[pos:])
        if m:
            pos += m.end(); continue
        for name, pat in TOKEN_RE:
            m = re.match(pat, text[pos:])
            if m:
                val = m.group(0)
                if name == 'ид' and val == 'TYPE': name = 'type'
                if name == 'ид' and val == 'if':   name = 'if'
                tokens.append((name, val))
                pos += m.end(); break
        else:
            raise SyntaxError(f"Неизвестный символ {text[pos]!r}")
    tokens.append(('$', '$'))
    return tokens

# ---------- 3. Рекурсивный спуск ----------
class Parser:
    def __init__(self, tokens):
        self.t = tokens
        self.i = 0

    def cur(self):
        return self.t[self.i]

    def eat(self, kind):
        tok = self.cur()
        if tok[0] != kind:
            raise SyntaxError(f"Ожидался {kind}, получен {tok}")
        self.i += 1
        return tok

    # S → if ( E ) S | type * , ид newline | ид = E newline
    def S(self):
        tok = self.cur()
        if tok[0] == 'if':
            self.eat('if'); self.eat('(')
            self.E()
            self.eat(')')
            self.S()                     # вложенный S сам съест newline
            print("  S → if ( E ) S")
        elif tok[0] == 'type':
            self.eat('type'); self.eat('*'); self.eat(',')
            name = self.eat('ид')[1]
            self.eat('newline')
            print(f"  S → type * , {name} newline")
        elif tok[0] == 'ид':
            name = self.eat('ид')[1]
            self.eat('=')
            self.E()
            self.eat('newline')
            print(f"  S → {name} = E newline")
        else:
            raise SyntaxError(f"Неожиданный токен в S: {tok}")

    # E → операнд { ('.'|'*'|',') операнд }   (проверка оператора — ПО ТАБЛИЦЕ)
    def E(self):
        self.operand()
        while self.cur()[0] in ('.', '*', ','):
            op = self.cur()
            if prec('ид', op[0]) != '<':
                break
            self.i += 1
            self.operand()
            print(f"  E → E {op[1]} операнд")

    def operand(self):
        tok = self.cur()
        if tok[0] == 'ид':
            self.eat('ид')
        elif tok[0] == 'конст':
            self.eat('конст')
        elif tok[0] == '(':
            self.eat('('); self.E(); self.eat(')')
        else:
            raise SyntaxError(f"Ожидался операнд, получен {tok}")

# ---------- 4. Чтение example.txt и запуск ----------
if __name__ == '__main__':
    with open("example.txt", "r", encoding="utf-8") as f:
        src = f.read()

    print("Исходный текст (example.txt):")
    print(src)

    toks = lex(src)
    print("Токены:")
    for t in toks:
        print("  ", t)
    print()

    p = Parser(toks)
    while p.cur()[0] != '$':
        p.S()

    print("\nРазбор завершён успешно.")