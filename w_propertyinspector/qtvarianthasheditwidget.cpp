#include "qtvarianthasheditwidget.h"
#include <QSpinBox>
#include <QScrollBar>
#include <QComboBox>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QSpacerItem>
#include <QStyleOption>
#include <QPainter>
#include <QMap>
#include <QTableWidget>
#include <QDialogButtonBox>

QtVariantHashEditWidget::QtVariantHashEditWidget(QWidget *parent) :
    QWidget(parent),
    m_edit(new QLineEdit),
    m_button(new QToolButton)
{
    QHBoxLayout *lt = new QHBoxLayout(this);
    enum { DecorationMargin = 4 };
    if (QApplication::layoutDirection() == Qt::LeftToRight)
        lt->setContentsMargins(DecorationMargin, 0, 0, 0);
    else
        lt->setContentsMargins(0, 0, DecorationMargin, 0);
    lt->setSpacing(0);
    lt->addWidget(m_edit);
    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_button->setFixedWidth(20);
    setFocusProxy(m_button);
    setFocusPolicy(m_button->focusPolicy());
    m_button->setText(tr("..."));
    m_button->installEventFilter(this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    lt->addWidget(m_button);
    m_edit->setReadOnly(false);
    m_edit->setText(m_text);
    connect(m_edit, &QLineEdit::textEdited , this, &QtVariantHashEditWidget::slotTextEdited);
}


void QtVariantHashEditWidget::setValue(const QString &c)
{
    if (m_text != c) {
        m_text = c;
        m_edit->setText(m_text);
    }
}

void  QtVariantHashEditWidget::slotTextEdited(const QString &value)
{
    m_text=value;
    emit valueChanged(m_text);
}

QString QVariantHashToQString(const QVariantHash &h);
QVariantHash QStringToQVariantHash(const QString &s);

void QtVariantHashEditWidget::buttonClicked()
{
    QDialog *d=new QDialog(parentWidget());

    QVBoxLayout *lt = new QVBoxLayout(d);
    lt->setSpacing(1);
    QTableWidget *t=new QTableWidget();
    t->setColumnCount(2);
    QVariantHash h=QStringToQVariantHash(m_text);
    t->setRowCount(h.count());
    int row=0;
    QStringList keys=h.keys();
    keys.sort();
    foreach (QString k, keys) {
        QTableWidgetItem *newItem = new QTableWidgetItem(k);
        t->setItem(row, 0, newItem);
        newItem = new QTableWidgetItem(h[k].toString());
        t->setItem(row, 1, newItem);
        row++;

    }


    lt->addWidget(t);
    QDialogButtonBox *buttonBox=new QDialogButtonBox(QDialogButtonBox::Ok
                                                     | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, d, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, d, &QDialog::reject);
    lt->addWidget(buttonBox);

    if (d->exec()==1){
        QVariantHash hh;
        for (int row=0;row<t->rowCount();row++){
            hh[t->item(row,0)->text()]=t->item(row,1)->text();
        }
        QString s=QVariantHashToQString(hh);
        if (s!=m_text){
            setValue(s);
            emit valueChanged(s);
        }
    }

    //    qDebug()<<"buttonClicked();";
    //    bool ok = false;
    //    QRgb oldRgba = m_color.rgba();


    //    if (ok && newRgba != oldRgba) {
    //        setValue(QColor::fromRgba(newRgba));
    //        emit valueChanged(m_color);
    //    }
}

bool QtVariantHashEditWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_button) {
        switch (ev->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
            switch (static_cast<const QKeyEvent*>(ev)->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                ev->ignore();
                return true;
            default:
                break;
            }
        }
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void QtVariantHashEditWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
