% Лабораторная работа № 2.3 «Синтаксический анализатор на основе
  предсказывающего анализа»
% <лабораторная ещё не сдана>
% Сергей Виленский, ИУ9-62Б

# Цель работы
Целью данной работы является изучение алгоритма построения таблиц предсказывающего анализатора.

# Индивидуальный вариант
```
' аксиома
<axiom <E>>
' правила грамматики
<E    <T E'>>
<E'   <+ T E'> <>>
<T    <F T'>>
<T'   <* F T'> <>>
<F    <n> <( E )>>
```

# Реализация

## Неформальное описание синтаксиса входного языка
Язык представления правил грамматики, в записи которых
правила и альтернативы правил обернуты в угловые скобки.

## Лексическая структура
Лекисческие домены в порядке возрастания приоритета.
```
Term    ::= [^a-zA-Z<>\s]
Nterm   ::= [a-zA-Z][^<>\s]*
Space   ::= \s
Comment ::= \'.*\n
```

## Грамматика языка
```
Rules     ::= Rule Rules | Comment Rules | ε.
Rule      ::= '<' Nterm '<' Altrule '>' Altrules '>'.
Altrules  ::= '<' Altrule '>' Altrules | ε.
Altrule   ::= Term Altrule | Nterm Altrule | ε.
Term      ::= [^a-zA-Z<>\s]
Nterm     ::= [a-zA-Z] Nterm'
Nterm'    ::= [^a-zA-Z<>\s] Nterm' | [a-zA-Z] Nterm' | ε
Comment   ::= \\' Commnet' \n
Comment'  ::= [^\n] Comment' | ε
```

## Таблица предсказывающего разбора

|         |'\\''|'\n'|[^a-zA-Z<>\s\\'\n]|[a-zA-Z]|'<'|'>'|$|
|:-       |:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|Rules    |Comment Rules|ERROR|ERROR|ERROR|Rule Rules|ERROR|ε|
|Rule     |ERROR|ERROR|ERROR|ERROR|'<' Nterm '<' Altrule '>' Altrules '>'|ERROR|ERROR|
|Altrules |ERROR|ERROR|ERROR|ERROR|'<' Altrule '>' Altrules|ε|ERROR|
|Altrule  |Term Altrule|Term Altrule|Term Altrule|Nterm Altrule|ERROR|ε|ERROR|
|Term     |[^a-zA-Z<>\s]|ε|[^a-zA-Z<>\s]|ERROR|ERROR|ERROR|ERROR|
|Nterm    |ERROR|ERROR|ERROR|[a-zA-Z] Nterm'|ε|ε|ERROR|
|Nterm'   |[^a-zA-Z<>\s]|ε|[^a-zA-Z<>\s] Nterm'|[a-zA-Z] Nterm'|ε|ε|ERROR
|Comment  |\\' Commnet' \n|ERROR|ERROR|ERROR|ERROR|ERROR|ERROR
|Comment' |[^\n] Comment'|ε|[^\n] Comment'|[^\n] Comment'|[^\n] Comment'|[^\n] Comment'|ε

## Программная реализация

```
…
```

# Тестирование

Входные данные

```
…
```

Вывод на `stdout`

```
…
```

# Вывод
‹пишете, чему научились›