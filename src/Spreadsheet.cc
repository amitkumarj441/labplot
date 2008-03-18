// LabPlot : Spreadsheet.cc

#include <KLocale>
#include <KDebug>
#include <QInputDialog>
#include <QHeaderView>
#include <QStandardItemModel>
#include <KAction>

#include "Spreadsheet.h"
#include "MainWin.h"
#include "TableModel.h"
#include "ColumnDialog.h"
#include "pixmaps/pixmap.h"
#include "column.h"

Spreadsheet::Spreadsheet(MainWin *mw)
	:QTableView(), mw(mw)
{
	kDebug()<<"Spreadsheet()"<<endl;
	type = SPREADSHEET;
	setModel(new TableModel());

	setColumnCount(2);
	setRowCount(100);
	setSortingEnabled(false);
	
	resetHeader();
	
/*	ld=0;
	graph=0;
	gtype=GRAPH2D;

	// find free worksheet number
	QWidgetList list = mw->getWorkspace()->windowList();
	int nr=0, sheets = mw->NrWorksheets()+mw->NrSpreadsheets();
	bool found_free=false;
	kDebug()<<"	Number of sheets "<<sheets<<endl;
	while(nr <= sheets && !found_free ) {
		nr++;
		found_free=true;
		for(int i=0;i<sheets;i++) {
			if(list.at(i) && list.at(i)->caption() == QString(i18n("Spreadsheet")+QString(" ")+QString::number(nr)))
				found_free=false;
		}
	}

	kDebug()<<"	Using number "<<nr<<endl;
	title = i18n("Spreadsheet")+QString(" ")+QString::number(nr);
	setCaption(title);
*/
	setWindowTitle("Spreadsheet");
	// TODO : set unique title "Spreadsheet 1", etc.

/*	table->setRowMovingEnabled (true);
	table->setColumnMovingEnabled (true);
	table->horizontalHeader()->installEventFilter(this );

	table->setFocusPolicy(QWidget::StrongFocus);
	table->setFocus();
	QObject::connect(table,SIGNAL(currentChanged(int,int)),this,SLOT(updateValuesDialog()));
	QObject::connect(table,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));

	if(mw) mw->updateSheetList();

	destination=-1;
	values_dialog=0;
	datafile=QString("");
*/
}

Spreadsheet::~Spreadsheet() {
	mw->updateGUI();
	delete model();
}

void Spreadsheet::contextMenuEvent(QContextMenuEvent *) {
	QMenu *menu = new QMenu(this);
	Menu(menu);
	menu->exec(QCursor::pos());
}

void Spreadsheet::Menu(QMenu *menu) {
	kDebug()<<"Spreadsheet::Menu()"<<endl;
	menu->clear();

	KAction *spreadaction = new KAction(KIcon(QIcon(spreadsheet_xpm)),i18n("New spreadsheet"),this);
	spreadaction->setShortcut(Qt::CTRL+Qt::Key_Equal);
	menu->addAction(spreadaction);
	// TODO
	// getting action from mw crashes
	// this crashes too
	//connect(spreadaction, SIGNAL(triggered()),mw, SLOT(newSpreadsheet()));
	// Doesn't work in main menu but in context menu
	connect(spreadaction, SIGNAL(triggered()),SLOT(newSpreadsheet()));

	// TODO
	KAction *action = new KAction(KIcon(QIcon(plot2d_xpm)),i18n("Plot"),this);
	action->setEnabled(false);
	menu->addAction(action);
	
	action = new KAction(KIcon("select"),i18n("Set title"),this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()),SLOT(setTitle()));
	action = new KAction(KIcon("select"),i18n("Set row count"),this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()),SLOT(setRowNumber()));
	action = new KAction(KIcon("select"),i18n("Add column"),this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()),SLOT(addColumn()));
	action = new KAction(KIcon("select"),i18n("Column properties"),this);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()),SLOT(setProperties()));
}

void Spreadsheet::newSpreadsheet() { mw->newSpreadsheet(); }

void Spreadsheet::setTitle(QString title) {
	kDebug()<<"Spreadsheet::setTitle("<<title<<")"<<endl;
	bool ok=true;
	if(title.isEmpty())
		title = QInputDialog::getText(this,"LabPlot", i18n("Spreadsheet title : "),QLineEdit::Normal, windowTitle(), &ok);

	if(ok && !title.isEmpty()) {
		setWindowTitle(title);
		//mw->updateSheetList();
	}
}

void Spreadsheet::setRowNumber(int row) {
	kDebug()<<"Spreadsheet::setRowNumber("<<row<<")"<<endl;
	bool ok=true;
	if(row<=0)
		row = QInputDialog::getInteger(this,"LabPlot", i18n("Row count : "), rowCount(), 1, INT_MAX, 1, &ok);

	if(ok && row>0)
		setRowCount(row);
}

QString Spreadsheet::text(int row, int col) const {
//	kDebug()<<"Spreadsheet::text("<<row<<col<<")"<<endl;
	QModelIndex index=model()->index(row,col);
	return model()->data(index).toString();
}

void Spreadsheet::setText(int row, int col, QString text) {
	QModelIndex index=model()->index(row,col);
	model()->setData(index,text);
}

void Spreadsheet::setNotes(QString t) {
	kDebug()<<"Spreadsheet::setNotes()"<<endl;
	bool ok=true;
	if(t.isEmpty())
		t = QInputDialog::getText(this,"LabPlot", i18n("Spreadsheet notes : "),QLineEdit::Normal, notes, &ok);
	if(!ok) return;

	notes = t;
}

QString Spreadsheet::columnHeader(int col) const {
//	kDebug()<<"Spreadsheet::columnheader("<<col<<")"<<endl;
	if(col<0) col=0; 
	return model()->headerData(col,Qt::Horizontal).toString(); 
} 

void Spreadsheet::setProperties(QString label, int type, int format) {
	if(label.isEmpty())
		(new ColumnDialog(mw,this))->show();
	else
		setColumnHeader(currentColumn(),label + ' ' + '{'+columnformatitems[format]+'}' + ' ' + '['+columntypeitems[type]+']');
}

void Spreadsheet::resetHeader(int from) {
	for (int col=from;col<columnCount();col++) {
		QString l;
		if(col==0)
			l=(QChar(col+65)+' '+'{'+columnformatitems[0]+'}'+' '+'['+columntypeitems[0]+']');
		else if(col<26)
			l=(QChar(col+65)+' '+'{'+columnformatitems[0]+'}'+' '+'['+columntypeitems[1]+']');
		else
			l=(QString(QChar(65+col/26-1)) +QString(QChar(65+col%26))+' '+'{'+columnformatitems[0]+'}'+' '+'['+columntypeitems[1]+']');
		setColumnHeader(col,l);
	}
}

QString Spreadsheet::columnName(int col) const {
	QString label = columnHeader(col);
	label.remove(QRegExp(" \\{.+\\]"));

	return label;
}

void Spreadsheet::setColumnName(int col, QString name) {
	kDebug()<<"setColumnName() : col="<<col<<" name = "<<name<<endl;
	if(col<0) return;
	QString header = columnHeader(col);
	header.replace(QRegExp(".*\\{"),name+" {");
	model()->setHeaderData(col,Qt::Horizontal,header);
}


QString Spreadsheet::columnType(int col) const {
	QString header = columnHeader(col);
	header.remove(QRegExp(".*\\["));
	header.remove(QRegExp("\\].*"));
	return header;
}

void Spreadsheet::setColumnType(int col, QString type) {
	kDebug()<<"setColumnType() : col="<<col<<" type = "<<type<<endl;
	if(col<0) return;
	QString label = columnHeader(col);
	label.replace(QRegExp(" \\[.+\\]"),QString(" ["+type+"]"));
	setColumnHeader(col, label);
}

QString Spreadsheet::columnFormat(int col) const {
	QString header = columnHeader(col);
	header.remove(QRegExp(".*\\{"));
	header.remove(QRegExp("\\}.*"));
	return header;
}

void Spreadsheet::setColumnFormat(int col, QString format) {
	kDebug()<<"setColumnFormat() : col="<<col<<" type = "<<format<<endl;
	if(col<0) return;
	QString label = columnHeader(col);
	label.replace(QRegExp(" \\{.+\\}"),QString(" {"+format+"}"));
	setColumnHeader(col, label);
}

int Spreadsheet::filledRows(int col) const {
	kDebug()<<"Spreadsheet::filledRows("<<col<<")"<<endl;
	if(col < 0 || col > columnCount())
		return -1;

	for(int row=rowCount()-1;row>=0;row--) {
		if(!text(row,col).isEmpty())
			return row;
	}
	return 0;
}

void Spreadsheet::addSet(Set *s) {
	kDebug()<<"Spreadsheet::addSet()"<<endl;

	int columns=0;
	switch(s->Type()) {
	case SET2D:
		columns=2;
		break;
	default: break;
	}

	if(columnCount()<columns)
		setColumnCount(columns);
	// add empty columns if necessary
	for(int i=0;i<columns;i++) {
		if(filledRows(columnCount()-columns)>1)
			setColumnCount(columnCount()+1);
	}

	if(s->Number() > rowCount())
		setRowCount(s->Number());

	// add data
	switch(s->Type()) {
	case SET2D: {
		Point *data = ((Set2D *)s)->Data();
		for(int i=0;i<s->Number();i++) {
			setText(i,columnCount()-2,QString::number(data[i].X(),'g',10));
			setText(i,columnCount()-1,QString::number(data[i].Y(),'g',10));
/*			if(data[i].Masked()) {
				xitem->setMasked();
				yitem->setMasked();
			}
*/
		}
		};break;
	default: break;
	}
}
