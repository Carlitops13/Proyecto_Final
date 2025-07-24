#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTableWidgetItem>
#include <QVector>
#include <cstdlib>
#include <ctime>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Gestión de Atletas");
    srand(time(nullptr));
    cargarAtletas();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_registrar_clicked()
{
    QString nombre = ui->lineEdit_nombre->text();
    QString deporte = ui->comboBox_deporte->currentText();

    if(nombre.isEmpty()){
        QMessageBox::warning(this, "Campo vacío", "El nombre no puede estar vacío");
        return;

    }

    static const QRegularExpression regex("^[a-zA-ZñÑáéíóúÁÉÍÓÚ\\s]+$");
    if (!regex.match(nombre).hasMatch()){
        QMessageBox::warning(this, "Dato Inválido","El nombre solo puede contener letras");
        return;
    }

    Atleta nuevoAtleta;
    nuevoAtleta.id = generarNuevoId();
    nuevoAtleta.nombre = formatearNombrePropio(nombre);
    nuevoAtleta.edad = ui->spinBox_edad->value();
    nuevoAtleta.deporte = formatearNombrePropio(deporte);

    QFile archivo("atletas.txt");
    if (archivo.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivo);

        out << nuevoAtleta.id << ":" << nuevoAtleta.nombre << ":" << nuevoAtleta.edad << ":" << nuevoAtleta.deporte << "\n";
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error de Archivo", "No se pudo abrir el archivo para registrar.");
        return;
    }

    cargarAtletas();
    ui->label_id->setText(QString::number(nuevoAtleta.id));
    ui->lineEdit_nombre->clear();
    ui->spinBox_edad->setValue(18);
    ui->comboBox_deporte->setCurrentIndex(0);
}


void MainWindow::on_tableWidget_atletas_itemClicked(QTableWidgetItem *item)
{
    if (!item) return;
    int fila = item->row();


    idSeleccionado = ui->tableWidget_atletas->item(fila, 0)->text();

    ui->label_id->setText("ID: " + idSeleccionado);
    ui->lineEdit_nombre->setText(ui->tableWidget_atletas->item(fila, 1)->text());
    ui->spinBox_edad->setValue(ui->tableWidget_atletas->item(fila, 2)->text().toInt());
    ui->comboBox_deporte->setCurrentText(ui->tableWidget_atletas->item(fila, 3)->text());
}
void MainWindow::limpiarCampos()
{
    idSeleccionado.clear();
    ui->label_id->setText("ID");
    ui->lineEdit_nombre->clear();
    ui->spinBox_edad->setValue(18);
    ui->comboBox_deporte->setCurrentIndex(0);
    ui->tableWidget_atletas->clearSelection();
}

void MainWindow::on_pushButton_limpiar_clicked()
{
    limpiarCampos();
}

void MainWindow::on_pushButton_actualizar_clicked()
{

    if (idSeleccionado.isEmpty()) {
        QMessageBox::warning(this, "Sin Selección", "Por favor, selecciona un atleta para actualizar.");
        return;
    }

    QString nuevoNombre = ui->lineEdit_nombre->text();
    if (nuevoNombre.isEmpty()) {
        QMessageBox::warning(this, "Campo Vacío", "El nombre no puede estar vacío.");
        return;
    }

    Atleta atletaActualizado;
    atletaActualizado.id = idSeleccionado.toInt();
    atletaActualizado.nombre = formatearNombrePropio(nuevoNombre);
    atletaActualizado.edad = ui->spinBox_edad->value();
    atletaActualizado.deporte = ui->comboBox_deporte->currentText();


    QFile archivo("atletas.txt");
    QStringList lineas;
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            lineas.append(in.readLine());
        }
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error", "No se pudo leer el archivo para actualizar.");
        return;
    }


    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&archivo);
        for (const QString &linea : lineas) {
            if (linea.startsWith(idSeleccionado + ":")) {

                out << atletaActualizado.id << ":" << atletaActualizado.nombre << ":" << atletaActualizado.edad << ":" << atletaActualizado.deporte << "\n";
            } else {

                out << linea << "\n";
            }
        }
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar el archivo actualizado.");
        return;
    }

    // 6. Refrescar la tabla y limpiar los campos
    cargarAtletas();
    limpiarCampos();
    QMessageBox::information(this, "Éxito", "Atleta actualizado correctamente.");
}

void MainWindow::on_pushButton_eliminar_clicked()
{
    if (idSeleccionado.isEmpty()) {
        QMessageBox::warning(this, "Sin Selección", "Por favor, selecciona un atleta de la tabla para eliminar.");
        return;
    }

    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Confirmar Eliminación", "¿Estás seguro de que quieres eliminar al atleta con ID " + idSeleccionado + "?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (respuesta == QMessageBox::No) {
        return;
    }

    QFile archivo("atletas.txt");
    QStringList lineas;
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            lineas.append(in.readLine());
        }
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error de Archivo", "No se pudo leer el archivo para eliminar.");
        return;
    }

    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&archivo);
        for (const QString &linea : lineas) {
            if (!linea.startsWith(idSeleccionado + ":")) {
                out << linea << "\n";
            }
        }
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error de Archivo", "No se pudo escribir en el archivo para eliminar.");
        return;
    }

    cargarAtletas();
    idSeleccionado.clear();
    ui->label_id->setText("ID");
    QMessageBox::information(this, "Eliminación Exitosa", "Atleta eliminado correctamente.");
}

void MainWindow::cargarAtletas()
{
    ui->tableWidget_atletas->setColumnCount(4);
    QStringList cabeceras= {"ID", "Nombre", "Edad", "Deporte"};
    ui->tableWidget_atletas->setHorizontalHeaderLabels(cabeceras);
    ui->tableWidget_atletas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QVector<Atleta> atletas;
    QFile archivo("atletas.txt");
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);

        while (!in.atEnd()) {
            QString linea = in.readLine();
            if(linea.isEmpty()) continue;
            QStringList separador = linea.split(':');
            if (separador.size() == 4) {
                Atleta atletaLeido;
                atletaLeido.id = separador[0].toInt();
                atletaLeido.nombre = separador[1];
                atletaLeido.edad = separador[2].toInt();
                atletaLeido.deporte = separador[3];
                atletas.append(atletaLeido);
            }
        }
        archivo.close();
    }

    ui->tableWidget_atletas->setRowCount(atletas.size());
    for (int i = 0; i < atletas.size(); ++i) {
        const auto& atleta = atletas[i];
        ui->tableWidget_atletas->setItem(i, 0, new QTableWidgetItem(QString::number(atleta.id)));
        ui->tableWidget_atletas->setItem(i, 1, new QTableWidgetItem(atleta.nombre));
        ui->tableWidget_atletas->setItem(i, 2, new QTableWidgetItem(QString::number(atleta.edad)));
        ui->tableWidget_atletas->setItem(i, 3, new QTableWidgetItem(atleta.deporte));
    }
}

int MainWindow::generarNuevoId()
{
    int idAleatorio;
    bool idExiste;
    do {
        idAleatorio = 1000 + rand() % 9000;
        idExiste = false;
        QFile archivo("atletas.txt");
        if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&archivo);
            while (!in.atEnd()) {
                QString linea = in.readLine();
                if (linea.startsWith(QString::number(idAleatorio) + ":")) {
                    idExiste = true;
                    break;
                }
            }
            archivo.close();
        }
    } while (idExiste);
    return idAleatorio;
}

QString MainWindow::formatearNombrePropio(const QString &texto)
{
    QStringList palabras = texto.split(' ', Qt::SkipEmptyParts);
    QString textoFormateado;
    for (QString &palabra : palabras) {
        if (!palabra.isEmpty()) {
            palabra = palabra.left(1).toUpper() + palabra.mid(1).toLower();
            textoFormateado += palabra + " ";
        }
    }
    return textoFormateado.trimmed();
}


void MainWindow::on_pushButton_borrarTodo_clicked()
{

    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::warning(this, "Confirmación Requerida",
                                 "¿Estás SEGURO de que quieres borrar TODOS los atletas?",
                                 QMessageBox::Yes | QMessageBox::No);


    if (respuesta== QMessageBox::No) {
        return;
    }

    QFile archivo("atletas.txt");

    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        archivo.close();
        QMessageBox::information(this, "Completado", "Todos los datos han sido eliminados.");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo acceder al archivo para borrar los datos.");
        return;
    }


    cargarAtletas();
    limpiarCampos();
}


void MainWindow::on_pushButton_guardar_clicked()
{
    if (ui->tableWidget_atletas->rowCount() == 0) {
        QMessageBox::information(this, "Tabla Vacía", "No hay atletas para guardar en el archivo.");
        return;
    }


    QString nombreArchivo = QFileDialog::getSaveFileName(this, "Guardar Lista de Atletas", "",  "Archivos de Texto (*.txt)");


    if (nombreArchivo.isEmpty()) {
        return;
    }


    QString contenido;


    contenido += "Lista de Atletas\n";
    contenido += "=================================================\n";
    contenido += "ID\tNombre\t\t\tEdad\tDeporte\n";
    contenido += "-------------------------------------------------\n";


    for (int fila = 0; fila < ui->tableWidget_atletas->rowCount(); ++fila) {
        QString id = ui->tableWidget_atletas->item(fila, 0)->text();
        QString nombre = ui->tableWidget_atletas->item(fila, 1)->text();
        QString edad = ui->tableWidget_atletas->item(fila, 2)->text();
        QString deporte = ui->tableWidget_atletas->item(fila, 3)->text();


        contenido += id + "\t" + nombre.leftJustified(20, ' ') + "\t" + edad + "\t" + deporte + "\n";
    }


    QFile archivo(nombreArchivo);
    if(archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&archivo);
        out << contenido;
        archivo.close();
        QMessageBox::information(this, "Guardado Exitoso", "La lista de atletas se ha guardado correctamente.");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar el archivo.");
    }
}
