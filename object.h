#ifndef OBJECT_H
#define OBJECT_H


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
class ServerButton;
class BlockMenu;

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



#endif // OBJECT_H
