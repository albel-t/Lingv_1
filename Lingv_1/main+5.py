# ============================================================
# main.py — Лабораторная 2.3 (рекурсивный спуск) + диалог (раздел 5)
# ============================================================
import re
import tkinter as tk
from tkinter import ttk
import openpyxl

# ============================================================
# 1. МАТРИЦА ПРЕДШЕСТВОВАНИЯ ИЗ EXCEL
# ============================================================
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

# ============================================================
# 2. ЛЕКСЕР
# ============================================================
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

# ============================================================
# 3. РЕКУРСИВНЫЙ СПУСК (вывод через колбэк self.out)
# ============================================================
class Parser:
    def __init__(self, tokens, out=print, arrow="→"):
        self.t = tokens
        self.i = 0
        self.out = out
        self.arrow = arrow

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
            self.S()
            self.out(f"  S {self.arrow} if ( E ) S")
        elif tok[0] == 'type':
            self.eat('type'); self.eat('*'); self.eat(',')
            name = self.eat('ид')[1]
            self.eat('newline')
            self.out(f"  S {self.arrow} type * , {name} newline")
            self.out(f"  S {self.arrow} type E newline")
        elif tok[0] == 'ид':
            name = self.eat('ид')[1]
            self.eat('=')
            self.E()
            self.eat('newline')
            self.out(f"  S {self.arrow} {name} = E newline")
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
            self.out(f"  E {self.arrow} E {op[1]} операнд")

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

# ============================================================
# 4. СОСТОЯНИЕ + ПРИЛОЖЕНИЕ
# ============================================================
class AppState:
    def __init__(self):
        self.separator = "→"      # символ для стрелок (по умолчанию)
        self.qa_answer = ""
        self.form_value = ""

state = AppState()


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Лабораторная: Организация диалога")
        self.geometry("820x620")
        self.minsize(680, 520)

        # ---------- Верх: меню-вкладки ----------
        self.notebook = ttk.Notebook(self)
        self.notebook.pack(side=tk.TOP, fill=tk.X, padx=8, pady=(8, 4))

        self.tab_qa = ttk.Frame(self.notebook)
        self.tab_form = ttk.Frame(self.notebook)
        self.tab_direct = ttk.Frame(self.notebook)

        self.notebook.add(self.tab_qa, text="Вопрос-ответ")
        self.notebook.add(self.tab_form, text="Заполнение бланка")
        self.notebook.add(self.tab_direct, text="Прямой режим")

        self._build_qa()
        self._build_form()
        self._build_direct()

        # ---------- Низ: вывод лабораторной работы ----------
        out_frame = ttk.LabelFrame(self, text="Вывод лабораторной работы")
        out_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=8, pady=4)

        self.output = tk.Text(out_frame, height=14, wrap=tk.WORD, state=tk.DISABLED)
        self.output.pack(fill=tk.BOTH, expand=True, padx=4, pady=4)

        # ---------- Самое низ: общее поле ввода ----------
        in_frame = ttk.Frame(self)
        in_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=8, pady=(0, 8))

        ttk.Label(in_frame, text="Ввод:").pack(side=tk.LEFT)
        self.entry = ttk.Entry(in_frame)
        self.entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(6, 6))
        self.entry.bind("<Return>", self._on_entry_return)

        ttk.Button(in_frame, text="Отправить", command=self._on_entry_return).pack(side=tk.RIGHT)

        self.log("Приложение запущено. Выберите вкладку сверху.")
        self.log("Прямой режим: /run — запустить лабораторную, /set_esp <строка> — разделитель.")

    # ---------- Вкладка 1: вопрос-ответ ----------
    def _build_qa(self):
        f = self.tab_qa
        ttk.Label(f, text="Какие символы использовать для стрелочек?").pack(anchor=tk.W, padx=8, pady=(10, 4))
        self.qa_entry = ttk.Entry(f)
        self.qa_entry.pack(fill=tk.X, padx=8, pady=4)
        ttk.Button(f, text="Ответить", command=self._qa_submit).pack(anchor=tk.W, padx=8, pady=(0, 10))

    def _qa_submit(self):
        ans = self.qa_entry.get().strip()
        if not ans:
            self.log("Вопрос-ответ: пустой ответ, попробуйте снова.")
            return
        state.qa_answer = ans
        state.separator = ans          # ответ на вопрос и есть символ стрелок
        self.log(f"Вопрос-ответ: получен ответ «{ans}». Разделитель для стрелок установлен.")

    # ---------- Вкладка 2: заполнение бланка ----------
    def _build_form(self):
        f = self.tab_form
        row = ttk.Frame(f)
        row.pack(anchor=tk.W, padx=8, pady=(10, 4), fill=tk.X)

        ttk.Label(row, text="для стрелки использовать").pack(side=tk.LEFT)
        self.form_entry = ttk.Entry(row, width=24)
        self.form_entry.pack(side=tk.LEFT, padx=6)
        ttk.Label(row, text="строку при выводе работы").pack(side=tk.LEFT)

        ttk.Button(f, text="Сохранить", command=self._form_submit).pack(
            anchor=tk.W, padx=8, pady=(6, 10)
        )

    def _form_submit(self):
        val = self.form_entry.get()
        state.form_value = val
        if val.strip() == "":
            self.log("Бланк: поле пустое, сохранять нечего.")
        else:
            state.separator = val
            self.log(f"Бланк: для стрелки использовать «{val}» строку при выводе работы. Разделитель установлен.")

    # ---------- Вкладка 3: прямой режим ----------
    def _build_direct(self):
        f = self.tab_direct
        ttk.Label(
            f,
            text="Прямой режим. Команды:\n/set_esp <строка> — установить разделитель\n/run — запустить лабораторную работу",
            justify=tk.LEFT,
        ).pack(anchor=tk.W, padx=8, pady=(10, 4))

    def _direct_command(self, cmd: str):
        cmd = cmd.strip()
        if not cmd:
            return
        if cmd.startswith("/set_esp"):
            parts = cmd.split(maxsplit=1)
            if len(parts) < 2 or not parts[1]:
                self.log(":системное уведомление: не указан разделитель. Используйте /set_esp <строка>")
                return
            state.separator = parts[1]
            self.log(f":системное уведомление: установлен разделитель {state.separator}")
        elif cmd == "/run":
            self._run_lab()
        else:
            self.log(f"Прямой режим: неизвестная команда «{cmd}».")

    # ---------- Запуск лабораторной (вывод в большое поле) ----------
    def _run_lab(self):
        sep = state.separator
        self.log(f":системное уведомление: запуск лабораторной работы, разделитель «{sep}»")

        try:
            with open("example.txt", "r", encoding="utf-8") as f:
                src = f.read()
        except FileNotFoundError:
            self.log("Ошибка: файл example.txt не найден рядом с main.py.")
            return

        self.log("Исходный текст (example.txt):")
        for line in src.splitlines():
            self.log("    " + line)

        try:
            toks = lex(src)
        except SyntaxError as e:
            self.log(f"Лексическая ошибка: {e}")
            return

        self.log("Токены:")
        for t in toks:
            self.log(f"    {t}")

        self.log("Разбор (рекурсивный спуск):")
        p = Parser(toks, out=self.log, arrow=sep)
        try:
            while p.cur()[0] != '$':
                p.S()
        except SyntaxError as e:
            self.log(f"Синтаксическая ошибка: {e}")
            return

        self.log("Разбор завершён успешно.")

    # ---------- Общее поле ввода ----------
    def _on_entry_return(self, event=None):
        text = self.entry.get().strip()
        if not text:
            return
        self.entry.delete(0, tk.END)

        active = self.notebook.index(self.notebook.select())
        if active == 0:
            self.qa_entry.delete(0, tk.END)
            self.qa_entry.insert(0, text)
            self._qa_submit()
        elif active == 1:
            self.form_entry.delete(0, tk.END)
            self.form_entry.insert(0, text)
            self._form_submit()
        else:
            self._direct_command(text)

    # ---------- Вывод ----------
    def log(self, msg: str):
        self.output.configure(state=tk.NORMAL)
        self.output.insert(tk.END, msg + "\n")
        self.output.see(tk.END)
        self.output.configure(state=tk.DISABLED)


if __name__ == "__main__":
    try:
        App().mainloop()
    except Exception:
        import traceback
        traceback.print_exc()
        input("Ошибка. Нажмите Enter...")