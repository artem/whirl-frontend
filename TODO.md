# TODO

- Чекер линеаризуемости за nlogn: http://rystsov.info/2017/07/16/linearizability-testing.html
- Fuzzing: https://colin-scott.github.io/blog/2015/10/07/fuzzing-raft-for-fun-and-profit/
- Режим бенчмарка (пререквизиты: аллокатор, моделирование диска)
- Партишены из Jepsen: https://github.com/jepsen-io/jepsen/blob/main/jepsen/src/jepsen/nemesis.clj
- Поддержка санитайзеров
- Вынести RPC / сериализацию в отдельную библиотеку
- Разделить кластер / конфигурацию: https://github.com/logcabin/logcabin
- Моделирование кипэлайва мешает детектить дедлоки в исполнении
- Поддержать cli для тестов - `--no-det-check`, `-—seed`
- Моделирование fs, лог поверх fs, тайминги для диска
- TrueTime - const в пределах одной итерации
- Параллельный запуск симуляций
