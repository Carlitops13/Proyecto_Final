#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTableWidgetItem>

struct Atleta {
    int id;
    QString nombre;
    int edad;
    QString deporte;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots: // Slots para los botones y la tabla
    void on_pushButton_registrar_clicked();
    void on_pushButton_eliminar_clicked();
    void on_pushButton_actualizar_clicked();
    // Vuelve a añadir o verifica que esta línea exista
    void on_tableWidget_atletas_itemClicked(QTableWidgetItem *item);
    void on_pushButton_limpiar_clicked();
     void on_pushButton_borrarTodo_clicked();
     void on_pushButton_guardar_clicked();


private:
    Ui::MainWindow *ui;
    QString idSeleccionado;
    void cargarAtletas();
    int generarNuevoId();
    QString formatearNombrePropio(const QString &texto);
    void limpiarCampos();
};
#endif // MAINWINDOW_H
