#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>

#include <QBrush>
#include <QComboBox>
#include <QCursor>
#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <vector>

#include <QMessageBox>

class Widget;
class MainWindow;
class RouterBlock;
class SwitchBlock;
class AddButton;
class BlockMenu;

class N_ScrollWidget : public QWidget
{
    Q_OBJECT

public:
    N_ScrollWidget(QWidget *parent = nullptr, QWidget *LinkedWidget = nullptr);
    ~N_ScrollWidget()
    {
        //delete LinkedWidget;
        delete T1;
    }
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QRect ScrollBody = {0, 0, 0, 0};
    QWidget *LinkedWidget = nullptr;
    QTimer *T1;

public slots:
    void Tick();
};

class N_MenuBLOCK : public QWidget
{
    Q_OBJECT

public:
    N_MenuBLOCK(QWidget *parent = nullptr, BlockMenu* menu=nullptr)
        : QWidget(parent), MenuFather(menu)
    {
    }

    void paintEvent(QPaintEvent *event){

        QPainter p(this);
        p.setBrush(QBrush(Qt::lightGray));
        p.drawRect(0, 0, this->width(), this->height());
    }
    ~N_MenuBLOCK();
    std::vector<QLineEdit *> EDITS;
    std::vector<QPushButton *> BUTTONS;
    std::vector<QLabel *> LABELS;
    std::vector<QComboBox *> COMBOS;
    int OTS;
    BlockMenu *MenuFather;

};

class WidgetM : public QWidget
{
    Q_OBJECT

public:
    WidgetM(QWidget *parent = nullptr, BlockMenu *MenuFather = nullptr);
    ~WidgetM();
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setGeometry(int x, int y, int w, int h);
    N_MenuBLOCK *CONTENT;
    N_ScrollWidget *SCROLL;
    BlockMenu *MenuFather;
};

class Object
{
public:
    Object(double width, double heigth, double left, double top)
    {
        TypeELEM="Object";
        this->width = width;
        this->heigth = heigth;
        this->left = left;
        this->top = top;
    }
    virtual ~Object()
    {
        for (int i = 0; child.size() != 0;) {
            //child[i]->~Object();
            if (child[i]!=nullptr)
                delete child[i];
            child.erase(child.begin());
        }
    }
    virtual void setSizeHARD(double width, double heigth)
    {
        this->width = width;
        this->heigth = heigth;
        updateGeometry();
    }
    virtual void _WIDGET_moveTo(double X, double Y) {}
    virtual void moveHARD(double deltaX, double deltaY)
    {
        this->left += deltaX;
        this->top += deltaY;
        updateGeometry();
    }
    virtual void moveToHARD(double X, double Y)
    {
        this->left = X;
        this->top = Y;
        updateGeometry();
    }
    virtual void updateGeometry()
    {
        for (auto now : child) {
            now->updateGeometry();
        }
    }
    void DrawFrame(QPainter *p)
    {
        int _BrushSize = 2;
        p->setPen(QPen(Qt::red, _BrushSize, Qt::SolidLine));
        p->drawRect(this->left, this->top, this->width, this->heigth);
    }
    virtual void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1))
    {
        //DrawFrame(Wind, p);
    }
    virtual void paintChild(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1))
    {
        if (frame==QRect(-1,-1,-1,-1)) frame=QRect(this->x(), this->y(), this->width, this->heigth);
        for (auto now : child) {
            p->setClipRect(frame);
            now->paint(Wind, p, Imag, frame);
        }
        for (auto now : child) {
            now->paintChild(Wind, p, Imag, frame);
        }
    }
    Object *whoIsDaddy(QPoint *Rec)
    {
        Object *currentDaddy = nullptr;
        if (this->left <= Rec->x() && this->left + this->width >= Rec->x() && this->top <= Rec->y()
            && this->top + this->heigth >= Rec->y()) {
            for (int i = 0; i < child.size() && currentDaddy == nullptr; i++) {
                if (!(this->child[i]->IsMenu)) {
                    currentDaddy = this->child[i]->whoIsDaddy(Rec);
                }
            }
            return currentDaddy == nullptr ? this : currentDaddy;
        } else
            return nullptr;
    }
    void addChildren(Object *SonOrDother) { child.push_back(SonOrDother); }
    void removeChildren(Object *Elem){
        auto it=std::find(child.begin(), child.end(), Elem);
        if (it!=child.end()) child.erase(it);
    }
    int w() { return width; }
    int h() { return heigth; }
    int x() { return left; }
    int y() { return top; }
    virtual void ClickBut() {}
    bool activate = 0;
    Object *parent = nullptr;
    Widget *LinkedItem = nullptr;
    double kofWidth, kofHeigth, kofLeft, kofTop;
    QString TypeELEM;

protected:
    bool IsBlock = 0, IsButton = 0, IsMenu = 0, isWorkspace = 0;
    double width = 0, heigth = 0, left = 0, top = 0;
    std::vector<Object *> child;
    friend Widget;
    friend RouterBlock;
    friend MainWindow;
};

class Widget : public Object
{
public:
    QString TypeOfBlock;
    Widget(double width,
           double heigth,
           double left,
           double top,
           Object *parent,
           QColor bcgColor = QColor(0, 0, 0))
        : Object(width * parent->width,
                 heigth * parent->heigth,
                 left * parent->width + parent->left,
                 top * parent->heigth + parent->top)
    {
        TypeELEM=("Widget");
        this->parent = parent;
        kofWidth = (double) this->width / this->parent->width;
        kofHeigth = (double) this->heigth / this->parent->heigth;
        kofLeft = ((double) this->left - this->parent->left) / this->parent->width;
        kofTop = ((double) this->top - this->parent->top) / this->parent->heigth;
        this->bcgColor = bcgColor;
    }
    void setSizeHARD(double width, double heigth) override
    {
        kofWidth = (double) width / this->parent->width;
        kofHeigth = (double) heigth / this->parent->heigth;
        updateGeometry();
    }
    void setSize(double width, double heigth)
    {
        this->width = this->parent->width * width;
        this->heigth = this->parent->heigth * heigth;
        kofWidth = this->width / this->parent->width;
        kofHeigth = this->heigth / this->parent->heigth;
        updateGeometry();
    }
    void move(double deltaX, double deltaY)
    {
        this->left += deltaX * this->parent->width;
        this->top += deltaY * this->parent->heigth;
        updateGeometry();
    }
    void moveHARD(double deltaX, double deltaY) override
    {
        this->left += deltaX;
        this->top += deltaY;
        kofLeft = ((double) this->left - this->parent->left) / this->parent->width;
        kofTop = ((double) this->top - this->parent->top) / this->parent->heigth;
        updateGeometry();
    }
    void moveTo(double X, double Y)
    {
        this->left = X * this->parent->width + parent->left;
        this->top = Y * this->parent->heigth + parent->top;
        updateGeometry();
    }
    void moveToHARD(double deltaX, double deltaY) override
    {
        this->left = deltaX;
        this->top = deltaY;
        kofLeft = ((double) this->left - this->parent->left) / this->parent->width;
        kofTop = ((double) this->top - this->parent->top) / this->parent->heigth;
        updateGeometry();
    }
    void updateGeometry() override
    {
        this->width = this->parent->width * kofWidth;
        this->heigth = this->parent->heigth * kofHeigth;
        this->left = kofLeft * this->parent->width + this->parent->left;
        this->top = this->parent->top + this->parent->heigth * kofTop;
        for (auto now : child) {
            now->updateGeometry();
        }
    }
    void _WIDGET_moveTo(double X, double Y) override
    {
        this->left = X * this->parent->width + parent->left;
        this->top = Y * this->parent->heigth + parent->top;
        updateGeometry();
    }
    friend RouterBlock;
    friend SwitchBlock;
    Widget *LinkedItem = nullptr;
    //bool isSizeAutochange = true;

    ~Widget(){
        if (LinkedItem!=nullptr)
            delete LinkedItem;
        LinkedItem=nullptr;
    }


protected:
    QColor bcgColor;
};

class ScrollBar;



class Scrollable : public Widget{
public:
    Scrollable(double width, double heigth, double left, double top, Object *parent, Widget* CONTENT);

    ~Scrollable(){
        //delete SCROLL;
        //delete CONTENT;
    }

    void updateGeometry() override {
        this->width = this->parent->w() * kofWidth;
        this->heigth = this->parent->h() * kofHeigth;
        this->left = kofLeft * this->parent->w() + this->parent->x();
        this->top = this->parent->y() + this->parent->h() * kofTop;
        CONTENT->kofWidth=1.0-12.0/parent->w();
        CONTENT->updateGeometry();
        child[1]->updateGeometry();
    }

    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        this->DrawFrame(p);
    }

    void paintChild(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        int up=frame.y(), down=frame.y()+frame.height();
        int left=frame.x(), right=frame.x()+frame.width();

        down=std::min(down, this->y()+this->h());
        right=std::min(right, this->x()+this->w());
        up=std::max(up, this->y());
        left=std::max(left, this->x());

        frame=QRect(QPoint(left, up), QPoint(right, down));
        for (auto now : child) {
            p->setClipRect(frame);
            now->paint(Wind, p, Imag, frame);
        }
        for (auto now : child) {
            now->paintChild(Wind, p, Imag, frame);
        }
    }


    void wheelEvent(QWheelEvent *event);
    Widget* CONTENT = nullptr;
    ScrollBar* SCROLL = nullptr;
};

class ScrollBar : public Widget
{

public:
    ScrollBar(Widget *parent = nullptr, Widget *LinkedWidget = nullptr);
    ~ScrollBar()
    {
        delete LinkedWidget;
        delete T1;
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        if (parent!=nullptr && parent->TypeELEM=="Scrollable"){
            Scrollable* par=(Scrollable*)this->parent;
            if (par->CONTENT->kofHeigth<=1) return;
        }
        //DrawFrame(p);
        p->setBrush(QBrush(Qt::gray));
        p->drawRect(this->x(), this->y(), this->w(), this->h());
        p->setBrush(QBrush(Qt::darkGray));
        p->drawRect(ScrollBody);
    }

    void updateGeometry() override{
        this->kofLeft=1.0-12.0/(parent->w());
        this->kofWidth=12.0/(parent->w());
        this->kofHeigth=1.0;
        this->kofTop=0.0;
        //this->moveToHARD(parent->x()+parent->w() - 12, parent->y()+0);
        //this->setSizeHARD(12, parent->h());
        this->width = this->parent->w() * kofWidth;
        this->heigth = this->parent->h() * kofHeigth;
        this->left = kofLeft * this->parent->w() + this->parent->x();
        this->top = this->parent->y() + this->parent->h() * kofTop;
        if (parent!=nullptr && parent->TypeELEM=="Scrollable"){
            Scrollable* par=(Scrollable*)this->parent;
            ScrollBody = {this->x(), this->y()-(par->CONTENT->kofTop)*this->h(), 10, std::max(2, h() * h() / par->CONTENT->h())};
        }
    }

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QRect ScrollBody = {0, 0, 0, 0};
    Widget *LinkedWidget = nullptr;
    QTimer *T1;


    void Tick();
};

class Window : public Widget
{
public:
    Window(double width,
           double heigth,
           double left,
           double top,
           Object *parent,
           QColor bcgColor = QColor(0, 0, 0),
           int Rad = 0)
        : Widget(width, heigth, left, top, parent, bcgColor)
    {
        this->Rad = Rad;
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage)
    {
        QPen k = p->pen();
        QBrush i = p->brush();
        //DrawFrame(Wind, p);
        p->setBrush(QBrush(bcgColor));
        p->setPen(QPen(bcgColor, 0, Qt::SolidLine));

        p->drawEllipse(left, top, Rad * 2, Rad * 2);
        p->drawEllipse(left, heigth + top - Rad * 2 - 1, Rad * 2, Rad * 2);
        p->drawEllipse(width + left - Rad * 2 - 1, top, Rad * 2, Rad * 2);
        p->drawEllipse(width + left - Rad * 2 - 1, heigth + top - Rad * 2, Rad * 2, Rad * 2);
        p->drawRect(left + Rad + 1, top, width - Rad * 2 - 2, heigth);
        p->drawRect(left, top + Rad, Rad * 2, heigth - Rad * 2);
        p->drawRect(left + width - Rad * 2, top + Rad, Rad * 2, heigth - Rad * 2);

        p->setPen(k);
        p->setPen(QPen(Qt::green, 2, Qt::SolidLine));

        p->drawArc(left, top, Rad * 2, Rad * 2, 90 * 16, 90 * 16);
        p->drawArc(left, heigth + top - Rad * 2 - 1, Rad * 2, Rad * 2, 180 * 16, 90 * 16);
        p->drawArc(width + left - Rad * 2 - 1, top, Rad * 2, Rad * 2, 360 * 16, 90 * 16);
        p->drawArc(width + left - Rad * 2 - 1,
                   heigth + top - Rad * 2,
                   Rad * 2,
                   Rad * 2,
                   270 * 16,
                   90 * 16);
        p->drawLine(left + Rad, top, left + width - Rad, top);
        p->drawLine(left + Rad, top + heigth, left + width - Rad, top + heigth);
        p->drawLine(left, top + Rad, left, top + heigth - Rad);
        p->drawLine(left + width, top + Rad, left + width, top + heigth - Rad);
        p->setPen(k);
        p->setBrush(i);
    }


    const QString TypeELEM="TestWindow";

protected:
    int Rad = 0;
};

class FuncPanel : public Widget
{
public:
    FuncPanel(double width, double heigth, double left, double top, Object *parent, QMainWindow *Wind)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        TypeELEM=("FuncPanel");
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        if (IsOpen)
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1, 647, 213, 243));
        else
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(214, 647, 53, 243));
    }
    bool IsOpen = 1;
};

class VariablePanel : public Widget
{
public:
    VariablePanel(
        double width, double heigth, double left, double top, Object *parent, QMainWindow *Wind)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {}
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        if (IsOpen)
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(269, 650, 213, 380));
        else
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(214, 647, 53, 380));
    }
    bool IsOpen = 1;
};

class Menu : public Object
{
public:
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1))
    {
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(0, 0, 1280, 644));
    }
    Menu(double width, double heigth, double left, double top)
        : Object(width, heigth, left, top)
    {}
};

class LeftMenu : public Widget
{
public:
    LeftMenu(double width, double heigth, double left, double top, Object *parent)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        TypeELEM = "LeftMenu";
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1281, 98, 101, 477));
        /*
        QString Text = "Func restrictions";
        //if (isHaveFont){
        QPen l = p->pen();
        QFont f = Wind->font();
        f.setPointSize(14);
        f.setBold(1);
        int wid, he;
        QFontMetrics FM(f);
        for (;;) {
            wid = FM.horizontalAdvance(Text);
            he = FM.height();
            if ((90 * w() / 98 >= wid && 18 * h() / 475 >= he) || f.pointSize() == 1) {
                break;
            }
            f.setPointSize(f.pointSize() - 1);
            QFontMetrics tmp(f);
            FM = tmp;
        }
        p->setFont(f);
        p->setPen(Qt::black);
        p->drawText(QRect(x() + (w() - FM.horizontalAdvance(Text)) / 3,
                          y() + (double) 6 * h() / 475,
                          90 * w() / 98,
                          18 * h() / 475),
                    Text);
        p->drawText(QRect(x() + (w() - FM.horizontalAdvance("Work with data")) / 3,
                          y() + (double) 113 * h() / 475,
                          90 * w() / 98,
                          15 * h() / 475),
                    "Work with data");
        p->drawText(QRect(x() + (w() - FM.horizontalAdvance("Inject algoritms")) / 3,
                          y() + (double) 267 * h() / 475,
                          90 * w() / 98,
                          15 * h() / 475),
                    "Inject algoritms");
        p->drawText(QRect(x() + (w() - FM.horizontalAdvance("Custom func")) / 3,
                          y() + (double) 376 * h() / 475,
                          90 * w() / 98,
                          15 * h() / 475),
                    "Custom func");
        p->setPen(l);
        */
        //p->drawText(QRect(x()+w()*0.1, y()+h()*0.01, w()*0.98, h()*0.1), "func restrictions");
    }
};

class PipeModButton : public Widget
{
public:
    PipeModButton(double width, double heigth, double left, double top, Object *parent)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        IsButton = 1;
        TypeOfButton = "PipeMod";
        TypeELEM = "PipeModButton";
    }
    bool isPipeMode=false;
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        if (isPipeMode==false){
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(836, 651, 169, 121));
        }else{
            p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1006, 652, 166, 119));
        }
        /*
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1384, 98, 70, 101));
        QFontMetrics FM(p->font());
        p->drawText(QRect(x() + (w() - FM.horizontalAdvance("Translate")) / 2,
                          y() + (h() - FM.height()) / 2,
                          FM.horizontalAdvance("Translate") + 1,
                          FM.height() + 1),
                    "Translate");
        */
    }

    void click(){
        isPipeMode=!isPipeMode;
    }
    /*void updateGeometry() override{
      this->width=this->parent->w()*kofWidth;
      this->heigth=this->parent->h()*kofHeigth;
      this->left=kofLeft*this->parent->w()+this->parent->x();
      this->top=this->parent->y()+this->parent->h()*kofTop;
      for (auto now : child){
        now->updateGeometry();
      }
    }*/
    QString TypeOfButton;
};

class ButtonOnBlock : public Widget
{
public:
    ButtonOnBlock(
        double width, double heigth, double left, double top, Object *parent, Widget *LinkedItem)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        IsButton = 1;
        TypeOfButton = "Block";
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        //if (!parent->activate)
        //  p->drawRect(left, top, width, heigth);
    }
    //click()
    //bool activate=0;
    QString TypeOfButton;
};

class EraseButton;

class DeleteButton : public QPushButton
{
    Q_OBJECT

public:
    DeleteButton(N_MenuBLOCK *parent = nullptr)
        : QPushButton(parent)
    {
        this->N_Parent = parent;
    }

    void mousePressEvent(QMouseEvent *event);
    N_MenuBLOCK *N_Parent;
};

class Workspace : public Widget
{
public:
    Workspace(double width,
              double heigth,
              double left,
              double top,
              Object *parent,
              Widget *LinkedItem,
              QMainWindow *Wind)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        isWorkspace = 1;
        this->LinkedItem = LinkedItem;
        TypeELEM = "Workspace";
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        //p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1280,0,273,97));
        QPen l = p->pen();
        p->setPen(Qt::black);
        p->drawRect(this->left, this->top, this->width, this->heigth);
        //p->drawText(QRect(x()+(w()-p->fontMetrics().horizontalAdvance(Text))/2, y()+(h()-p->fontMetrics().height())/2, 100, p->fontMetrics().height()), Text);
        p->setPen(l);
        //p->drawRect(left, top, width, heigth);
        //LinkedItem->paint(Wind, p, Imag);
        //BlockButton->paint(Wind, p, Imag);
    }




};

class BlockMenu : public Widget
{
public:
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        if (LinkedItem->activate) {
            //p->drawRect(left, top, width, heigth);
            if (LinkedItem->TypeOfBlock == "Switch" || LinkedItem->TypeOfBlock == "Router") {
                p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1383, 317, 413, 201));
                QPen l = p->pen();
                QFont f = Wind->font();
                f.setPointSize(14);
                f.setBold(1);
                int wid, he;
                QFontMetrics FM(f);
                for (;;) {
                    wid = FM.horizontalAdvance("Ports");
                    he = FM.height();
                    if (((1 - 0.3415) * h() >= wid && 0.08716 * w() >= he)
                        || f.pointSize() == 1) {
                        break;
                    }
                    f.setPointSize(f.pointSize() - 1);
                    QFontMetrics tmp(f);
                    FM = tmp;
                }
                p->setFont(f);
                p->setPen(Qt::black);
                p->rotate(-90);
                p->drawText(QRect(y() * (-1) - h() + ((1 - 0.3415) * h() - wid) / 2,
                                  x(),
                                  wid,
                                  he),
                            "Ports");
                //p->drawLine (y()*(-1)-0.3415*h(), x()-100, y()*(-1)-0.3415*h(), x()+100);
                //p->drawLine (y()*(-1)-0.3415*h()-100, x(), y()*(-1)-0.3415*h()+100, x());
                p->rotate(90);
                p->setPen(l);
                UserMenu->show();
            } else if (LinkedItem->TypeOfBlock == "Server") {
                p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1382, 766, 413, 250));
                QPen l = p->pen();
                QFont f = Wind->font();
                f.setPointSize(14);
                f.setBold(1);
                int wid, he;
                QFontMetrics FM(f);
                for (;;) {
                    wid = FM.horizontalAdvance("Ports");
                    he = FM.height();
                    if (((1 - 0.482) * h() >= wid && 0.08716 * w() >= he) || f.pointSize() == 1) {
                        break;
                    }
                    f.setPointSize(f.pointSize() - 1);
                    QFontMetrics tmp(f);
                    FM = tmp;
                }
                p->setFont(f);
                p->setPen(Qt::black);
                p->rotate(-90);
                p->drawText(QRect(y() * (-1) - h() + ((1 - 0.482) * h() - wid) / 2, x(), wid, he),
                            "Ports");
                //p->drawLine (y()*(-1)-0.3415*h(), x()-100, y()*(-1)-0.3415*h(), x()+100);
                //p->drawLine (y()*(-1)-0.3415*h()-100, x(), y()*(-1)-0.3415*h()+100, x());
                p->rotate(90);
                p->setPen(l);
            }
        }
    }
    BlockMenu(double width,
              double heigth,
              double left,
              double top,
              Object *parent,
              Widget *LinkedItem,
              QMainWindow *Wind)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {
        IsMenu = 1;
        this->LinkedItem = LinkedItem;
        UserMenu = new N_MenuBLOCK(Wind, this);
        UserMenu->setGeometry((int) (this->w() * 0.11 + this->x()),
                              (int) (this->h() * 0.02 + this->y()),
                              (int) (this->w() * 0.88),
                              (int) (this->h() * 0.96));
        UserMenu->BUTTONS.push_back(new DeleteButton(UserMenu));
        UserMenu->BUTTONS[0]->text()="DELETE";
        UserMenu->BUTTONS[0]->move(UserMenu->width() * 0.25/2, 0.70*heigth);
        UserMenu->BUTTONS[0]->resize(UserMenu->width() * 0.75, UserMenu->height() * 0.2);
        UserMenu->hide();
        updateGeometry();
    }

    void updateGeometry() override
    {
        this->width = this->parent->w() * kofWidth;
        this->heigth = this->parent->h() * kofHeigth;
        this->left = kofLeft * this->parent->w() + this->parent->x();
        this->top = this->parent->y() + this->parent->h() * kofTop;
        for (auto now : child) {
            now->updateGeometry();
        }
        UserMenu->setGeometry((int) (this->w() * 0.11 + this->x()),
                              (int) (this->h() * 0.02 + this->y()),
                              (int) (this->w() * 0.88),
                              (int) (this->h() * 0.96));
        UserMenu->BUTTONS[0]->move(UserMenu->width() * 0.25/2, 0.70*heigth);
        UserMenu->BUTTONS[0]->resize(UserMenu->width() * 0.75, UserMenu->height() * 0.2);
    }

    ~BlockMenu(){
        LinkedItem = nullptr;
    }

    N_MenuBLOCK *UserMenu = nullptr;
};

class RouterBlock : public Widget
{
public:
    int Num=12;
    RouterBlock(
        double width, double heigth, double left, double top, Object *parent, QMainWindow *Wind)
        : Widget(width, heigth, left, top, parent, QColor(0, 0, 0))
    {

        this->Wind = Wind;
        IsBlock = 1;
        TypeELEM = "Begin";
        TypeOfBlock = "Begin";
        //removeChildren(BlockButton);
        //delete BlockButton;
        BlockButton = new ButtonOnBlock(0.171429, 0.823834, 0.828, 0.144, this, this);
        //BlockButton->setSize(0.172, 0.7196);
        //addChildren(new Window(0.5, 0.5, 0.5, 0.5, this));

        //removeChildren(LinkedItem);
        //delete LinkedItem;
        LinkedItem = new BlockMenu(1.67876, 2.2, 0.823834, 0.13, this, this, Wind);
        addChildren(BlockButton);
        addChildren(LinkedItem);
    }



    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1280, 0, 273, 97));
        p->drawImage(QRect(x()+0.46402*w(), y()+h()*0.37/2, w()*0.343, h()*0.63), Imag, QRect(615, 650, 123, 88));
        //SERVER ICON p->drawImage(QRect(x()+0.48139*w(), y()+h()*0.11/2, w()*0.194, h()*0.89), Imag, QRect(535, 650, 78, 126));
        QString Text = "Router";
        Text+=(Num ? " "+QString::number(Num): "");
        QFont f = Wind->font();
        f.setPointSize(13);
        f.setBold(1);
        int wid, he;
        QRect TextBound(x()+w()*0.044665, y()+h()*0.29078, w()*(0.46402-0.045), h()*0.411348);
        QFontMetrics FM(f);
        for (;;) {
            wid = FM.horizontalAdvance(Text);
            he = FM.height();
            //updateGeometry();
            if ((TextBound.width() >= wid && TextBound.height() >= he) || f.pointSize() == 1) {
                break;
            }
            f.setPointSize(f.pointSize() - 1);
            QFontMetrics tmp(f);
            FM = tmp;
        }
        p->setFont(f);
        QPen l = p->pen();
        p->setPen(Qt::black);
        p->drawText(QRect(TextBound.x() + (TextBound.width() - p->fontMetrics().horizontalAdvance(Text)) / 2,
                          TextBound.y() + (TextBound.height() - p->fontMetrics().height()) / 2,
                          100,
                          p->fontMetrics().height()),
                    Text);
        p->setPen(l);
    }
    void move(double deltaX, double deltaY)
    {
        this->left += deltaX * this->parent->w();
        this->top += deltaY * this->parent->h();
        updateGeometry();
        //LinkedItem->move(deltaX, deltaY);
    }
    void moveHARD(double deltaX, double deltaY) override
    {
        this->left += deltaX;
        this->top += deltaY;
        kofLeft = ((double) this->left - this->parent->x()) / this->parent->w();
        kofTop = ((double) this->top - this->parent->y()) / this->parent->h();
        this->LinkedItem->left += deltaX;
        this->LinkedItem->top += deltaY;
        LinkedItem->kofLeft = ((double) this->LinkedItem->x() - this->LinkedItem->parent->x())
                              / this->LinkedItem->parent->w();
        LinkedItem->kofTop = ((double) this->LinkedItem->y() - this->LinkedItem->parent->y())
                             / this->LinkedItem->parent->h();
        LinkedItem->updateGeometry();
        this->BlockButton->left += deltaX;
        this->BlockButton->top += deltaY;
        BlockButton->kofLeft = ((double) this->BlockButton->x() - this->BlockButton->parent->x())
                               / this->BlockButton->parent->w();
        BlockButton->kofTop = ((double) this->BlockButton->y() - this->BlockButton->parent->y())
                              / this->BlockButton->parent->h();
        //updateGeometry();
    }
    ~RouterBlock() override
    {
        for (int i = 0; child.size() != 0;) {
            //child[i]->~Object();
            if (child[i]!=nullptr)
                delete child[i];
            child.erase(child.begin());
        }
    }
    void ClickBut()
    {
        if (this->activate) {
            LinkedItem->UserMenu->show();

            //QMessageBox::critical(Wind, "Block", "show");
        } else {
            LinkedItem->UserMenu->hide();
            //QMessageBox::critical(Wind, "Block", "hide");
        }
    }
    QMainWindow *Wind;
    BlockMenu *LinkedItem;
    ButtonOnBlock *BlockButton;
};

class SwitchBlock : public RouterBlock
{
public:
    int Num=12;
    SwitchBlock(
        double width, double heigth, double left, double top, Object *parent, QMainWindow *Wind)
        : RouterBlock(width, heigth, left, top, parent, Wind)
    {

        this->Wind = Wind;
        IsBlock = 1;
        TypeELEM = "End";
        TypeOfBlock = "End";

        removeChildren(BlockButton);
        delete BlockButton;
        BlockButton = new ButtonOnBlock(0.171429, 0.823834, 0.828, 0.144, this, this);
        //BlockButton->setSize(0.172, 0.7196);
        //addChildren(new Window(0.5, 0.5, 0.5, 0.5, this));

        removeChildren(LinkedItem);
        delete LinkedItem;
        LinkedItem = new BlockMenu(1.67876, 2.2, 0.823834, 0.13, this, this, Wind);
        addChildren(BlockButton);
        addChildren(LinkedItem);
    }



    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1280, 0, 273, 97));
        p->drawImage(QRect(x()+0.44665*w(), y()+h()*0.51/2, w()*0.369727, h()*0.49), Imag, QRect(535, 778, 154, 68));
        //SERVER ICON p->drawImage(QRect(x()+0.48139*w(), y()+h()*0.11/2, w()*0.194, h()*0.89), Imag, QRect(535, 650, 78, 126));
        QString Text = "Switch";
        Text+=(Num ? " "+QString::number(Num): "");
        QFont f = Wind->font();
        f.setPointSize(13);
        f.setBold(1);
        int wid, he;
        QRect TextBound(x()+w()*0.044665, y()+h()*0.29078, w()*(0.44665-0.045), h()*0.411348);
        QFontMetrics FM(f);
        for (;;) {
            wid = FM.horizontalAdvance(Text);
            he = FM.height();
            //updateGeometry();
            if ((TextBound.width() >= wid && TextBound.height() >= he) || f.pointSize() == 1) {
                break;
            }
            f.setPointSize(f.pointSize() - 1);
            QFontMetrics tmp(f);
            FM = tmp;
        }
        p->setFont(f);
        QPen l = p->pen();
        p->setPen(Qt::black);
        p->drawText(QRect(TextBound.x() + (TextBound.width() - p->fontMetrics().horizontalAdvance(Text)) / 2,
                          TextBound.y() + (TextBound.height() - p->fontMetrics().height()) / 2,
                          100,
                          p->fontMetrics().height()),
                    Text);
        p->setPen(l);
    }
};

class ServerBlock : public RouterBlock
{
public:
    ServerBlock(
        double width, double heigth, double left, double top, Object *parent, QMainWindow *Wind)
        : RouterBlock(width, heigth, left, top, parent, Wind)
    {
        TypeOfBlock = "Match";
        TypeELEM = "Match";

        removeChildren(BlockButton);
        delete BlockButton;
        BlockButton = new ButtonOnBlock(0.16, 1, 0.840, 0, this, this);
        this->child[0] = BlockButton;
        //BlockButton->moveTo(0.85, 0);

        //BlockButton->setSize(1, 1);
        //BlockButton->updateGeometry();

        removeChildren(LinkedItem);
        delete LinkedItem;
        LinkedItem = new BlockMenu(1.583,
                                   2.1,
                                   1 - (double) BlockButton->kofWidth * this->width / this->width,
                                   (double) BlockButton->kofTop * this->heigth / this->heigth,
                                   this,
                                   this,
                                   Wind);
        this->child[1] = LinkedItem;
    }
    void paint(QMainWindow *Wind, QPainter *p, QImage Imag, QRect frame=QRect(-1,-1,-1,-1)) override
    {
        p->drawImage(QRect(x(), y(), w(), h()), Imag, QRect(1454, 98, 299, 118));
        p->drawImage(QRect(x()+0.5711*w(), y()+h()*0.2/2, w()*0.18578, h()*0.8), Imag, QRect(535, 650, 78, 126));
        QString Text = "Server";
        Text+=(Num ? " "+QString::number(Num): "");
        QFont f = Wind->font();
        f.setPointSize(13);
        f.setBold(1);
        int wid, he;
        QRect TextBound(x()+w()*0.01376, y()+h()*0.533333/2, w()*(0.5711-0.014), h()*0.46666);
        QFontMetrics FM(f);
        for (;;) {
            wid = FM.horizontalAdvance(Text);
            he = FM.height();
            //updateGeometry();
            if ((TextBound.width() >= wid && TextBound.height() >= he) || f.pointSize() == 1) {
                break;
            }
            f.setPointSize(f.pointSize() - 1);
            QFontMetrics tmp(f);
            FM = tmp;
        }
        p->setFont(f);
        QPen l = p->pen();
        p->setPen(Qt::black);
        p->drawText(QRect(TextBound.x() + (TextBound.width() - p->fontMetrics().horizontalAdvance(Text)) / 2,
                          TextBound.y() + (TextBound.height() - p->fontMetrics().height()) / 2,
                          100,
                          p->fontMetrics().height()),
                    Text);
        p->setPen(l);
    }
    QString Text;
};

#endif // OBJECTS_H
