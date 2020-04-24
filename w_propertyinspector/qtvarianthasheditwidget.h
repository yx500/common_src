#ifndef QTVARIANTHASHEDITWIDGET_H
#define QTVARIANTHASHEDITWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>


class QtVariantHashEditWidget : public QWidget {
    Q_OBJECT

public:
    QtVariantHashEditWidget(QWidget *parent);
    virtual ~QtVariantHashEditWidget(){}

    bool eventFilter(QObject *obj, QEvent *ev);
    QString m_text;

public Q_SLOTS:
    void setValue(const QString &value);

Q_SIGNALS:
    void valueChanged(const QString &value);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();
    void slotTextEdited(const QString &value);

private:

    QLineEdit *m_edit;
    QToolButton *m_button;
};

#endif // QTVARIANTHASHEDITWIDGET_H
