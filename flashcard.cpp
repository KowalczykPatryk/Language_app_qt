#include "flashcard.h"

Flashcard::Flashcard(const QString &question, const QString &answer)
    : question(question), answer(answer) {}

QString Flashcard::getQuestion() const
{
    return question;
}
QString Flashcard::getAnswer() const
{
    return answer;
}
