/***************************************************************************
    File                 : matrixcommands.h
    Project              : SciDAVis
    Description          : Commands used in Matrix (part of the undo/redo framework)
    --------------------------------------------------------------------
    Copyright            : (C) 2015 Alexander Semke (alexander.semke@web.de)
    Copyright            : (C) 2008 Tilman Benkert (thzs@gmx.net)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef MATRIX_COMMANDS_H
#define MATRIX_COMMANDS_H

#include <QUndoCommand>
#include <QVector>
#include "Matrix.h"

//! Insert columns
class MatrixInsertColumnsCmd : public QUndoCommand {
public:
	MatrixInsertColumnsCmd(MatrixPrivate* private_obj, int before, int count, QUndoCommand* parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! Column to insert before
	int m_before;
	//! The number of new columns
	int m_count;
};

//! Insert rows
class MatrixInsertRowsCmd : public QUndoCommand {
public:
	MatrixInsertRowsCmd(MatrixPrivate* private_obj, int before, int count, QUndoCommand* parent = 0);

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate* m_private_obj;
	//! Row to insert before
	int m_before;
	//! The number of new rows
	int m_count;
};

//! Remove columns
class MatrixRemoveColumnsCmd : public QUndoCommand {
public:
	MatrixRemoveColumnsCmd(MatrixPrivate* private_obj, int first, int count, QUndoCommand* parent = 0);

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! First column to remove
	int m_first;
	//! The number of columns to remove
	int m_count;
	//! Backups of the removed columns
	QVector< QVector<double> > m_backups;
};

//! Remove rows
class MatrixRemoveRowsCmd : public QUndoCommand {
public:
	MatrixRemoveRowsCmd(MatrixPrivate* private_obj, int first, int count, QUndoCommand* parent = 0);

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! First row to remove
	int m_first;
	//! The number of rows to remove
	int m_count;
	//! Backups of the removed rows
	QVector< QVector<double> > m_backups;
};


///////////////////////////////////////////////////////////////////////////
// class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////
//! Clear matrix
class MatrixClearCmd : public QUndoCommand
{
public:
	explicit MatrixClearCmd( MatrixPrivate * private_obj, QUndoCommand * parent = 0 );
	~MatrixClearCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! Backups of the cleared cells
	QVector< QVector<double> > m_backups;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////
//! Clear matrix column
class MatrixClearColumnCmd : public QUndoCommand
{
public:
	MatrixClearColumnCmd( MatrixPrivate * private_obj, int col, QUndoCommand * parent = 0 );
	~MatrixClearColumnCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! The index of the column
	int m_col;
	//! Backup of the cleared column
	QVector<double> m_backup;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetCellValueCmd
///////////////////////////////////////////////////////////////////////////
//! Set cell value
class MatrixSetCellValueCmd : public QUndoCommand
{
public:
	MatrixSetCellValueCmd( MatrixPrivate * private_obj, int row, int col, double value, QUndoCommand * parent = 0 );
	~MatrixSetCellValueCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! The index of the row
	int m_row;
	//! The index of the column
	int m_col;
	//! New cell value
	double m_value;
	//! Backup of the changed value
	double m_old_value;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetCellValueCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetCoordinatesCmd
///////////////////////////////////////////////////////////////////////////
//! Set matrix coordinates
class MatrixSetCoordinatesCmd : public QUndoCommand
{
public:
	MatrixSetCoordinatesCmd( MatrixPrivate * private_obj, double x1, double x2, double y1, double y2, QUndoCommand * parent = 0 );
	~MatrixSetCoordinatesCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	double m_new_x1;
	double m_new_x2;
	double m_new_y1;
	double m_new_y2;
	double m_old_x1;
	double m_old_x2;
	double m_old_y1;
	double m_old_y2;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetCoordinatesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
//! Set matrix formula
class MatrixSetFormulaCmd : public QUndoCommand
{
	public:
		MatrixSetFormulaCmd( MatrixPrivate * private_obj, QString formula);

		virtual void redo();
		virtual void undo();

	private:
		//! The private object to modify
		MatrixPrivate * m_private_obj;
		QString m_other_formula;
};
///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetColumnCellsCmd
///////////////////////////////////////////////////////////////////////////
//! Set cell values for (a part of) a column at once
class MatrixSetColumnCellsCmd : public QUndoCommand
{
public:
	MatrixSetColumnCellsCmd( MatrixPrivate * private_obj, int col, int first_row, int last_row, const QVector<double> & values, QUndoCommand * parent = 0 );
	~MatrixSetColumnCellsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! The index of the column
	int m_col;
	//! The index of the first row
	int m_first_row;
	//! The index of the last row
	int m_last_row;
	//! New cell values
	QVector<double> m_values;
	//! Backup of the changed values
	QVector<double> m_old_values;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetColumnCellsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetRowCellsCmd
///////////////////////////////////////////////////////////////////////////
//! Set cell values for (a part of) a row at once
class MatrixSetRowCellsCmd : public QUndoCommand
{
public:
	MatrixSetRowCellsCmd( MatrixPrivate * private_obj, int row, int first_column, int last_column, const QVector<double> & values, QUndoCommand * parent = 0 );
	~MatrixSetRowCellsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
	//! The index of the row
	int m_row;
	//! The index of the first column
	int m_first_column;
	//! The index of the last column
	int m_last_column;
	//! New cell values
	QVector<double> m_values;
	//! Backup of the changed values
	QVector<double> m_old_values;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetRowCellsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixTransposeCmd
///////////////////////////////////////////////////////////////////////////
//! Transpose the matrix
class MatrixTransposeCmd : public QUndoCommand
{
public:
	explicit MatrixTransposeCmd( MatrixPrivate * private_obj, QUndoCommand * parent = 0 );
	~MatrixTransposeCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixTransposeCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixMirrorHorizontallyCmd
///////////////////////////////////////////////////////////////////////////
//! Mirror the matrix horizontally
class MatrixMirrorHorizontallyCmd : public QUndoCommand
{
public:
	explicit MatrixMirrorHorizontallyCmd( MatrixPrivate * private_obj, QUndoCommand * parent = 0 );
	~MatrixMirrorHorizontallyCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixMirrorHorizontallyCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixMirrorVerticallyCmd
///////////////////////////////////////////////////////////////////////////
//! Mirror the matrix vertically
class MatrixMirrorVerticallyCmd : public QUndoCommand
{
public:
	explicit MatrixMirrorVerticallyCmd( MatrixPrivate * private_obj, QUndoCommand * parent = 0 );
	~MatrixMirrorVerticallyCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	MatrixPrivate * m_private_obj;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixMirrorVerticallyCmd
///////////////////////////////////////////////////////////////////////////



#endif // MATRIX_COMMANDS_H
