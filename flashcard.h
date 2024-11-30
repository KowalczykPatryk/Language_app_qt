#ifndef FLASHCARD_H
#define FLASHCARD_H
#include <QString>

class Flashcard
{
public:
    Flashcard(const QString &question, const QString &answer);
    QString getQuestion() const;
    QString getAnswer() const;
private:
    QString question;
    QString answer;
};

#endif // FLASHCARD_H
