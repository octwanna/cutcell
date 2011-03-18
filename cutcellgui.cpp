/* Cartesian cut cell grid generator: GUI interface
 *
 * Given an OFF file and grid parameters, cut a grid and output the CGNS
 * representation of the resulting cartesian cut cell grid.
 *
 * Copyright 2010 Bruce Duncan, University of Edinburgh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cutcell.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <QApplication>
#include <cutcellgui.hpp>

cutcell::gui::cutcellgui(QWidget *parent) {
    setupUi(this);

    connect(generateButton, SIGNAL(clicked()), this, SLOT(generateSlot()));
    connect(offOpenButton, SIGNAL(clicked()), this, SLOT(getInFile()));
    connect(cgnsOpenButton, SIGNAL(clicked()), this, SLOT(getOutFile()));
}

void cutcell::gui::cutcellgui::getInFile()
{
    QString path;

    path = QFileDialog::getOpenFileName(
        this,
        "Choose a file to open",
        QString::null,
        QString::null);

    offFileLineEdit->setText( path );
}

void cutcell::gui::cutcellgui::getOutFile()
{
    QString path;

    path = QFileDialog::getOpenFileName(
        this,
        "Choose a file to write",
        QString::null,
        QString::null);

    cgnsFileLineEdit->setText( path );
}

void cutcell::gui::cutcellgui::generateSlot() {
    cutcell::Polyhedron P;
    // Read the OFF format from the input file or stdin.
    std::ifstream in(offFileLineEdit->text());
    in >> P; // parse the file as a Nef_polyhedron object.
    cutcell::Nef_polyhedron N1(P);
    // Transform the object accordingly.
    cutcell::Aff_transformation Aff1(cutcell::TRANSLATION,
                                     cutcell::Vector(translationXSpinBox->value(),
                                                     translationYSpinBox->value(),
                                                     translationZSpinBox->value()));
    cutcell::Aff_transformation Aff2(cutcell::SCALING, scalingSpinBox->value());
    N1.transform(Aff1);
    N1.transform(Aff2);

    // Create the cutting grid.
    cutcell::Grid grid(grixSizeXSpinBox->value(), grixSizeYSpinBox->value(), grixSizeZSpinBox->value());

    // Cut the grid to the object.
    grid.cut(N1);

    // Output the grid in CGNS format.
    grid.output_cgns_file(cgnsFileLineEdit->text());
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    cutcell::gui::cutcellgui *window = new cutcell::gui::cutcellgui;

    window->show();
    return app.exec();
}