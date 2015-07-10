#include "templateedit.h"
#include "printoptions.h"
#include "printer.h"
#include "ui_templateedit.h"

#include <QMessageBox>

TemplateEdit::TemplateEdit(QWidget *parent, struct print_options *printOptions, struct template_options *templateOptions) :
	QDialog(parent),
	ui(new Ui::TemplateEdit)
{
	ui->setupUi(this);
	this->templateOptions = templateOptions;
	newTemplateOptions = *templateOptions;
	this->printOptions = printOptions;

	// restore the settings and init the UI
	ui->fontSelection->setCurrentIndex(templateOptions->font_index);
	ui->fontsize->setValue(templateOptions->font_size);
	ui->colorpalette->setCurrentIndex(templateOptions->color_palette_index);
	ui->linespacing->setValue(templateOptions->line_spacing);

	if (printOptions->p_template == print_options::ONE_DIVE) {
		grantlee_template = TemplateLayout::readTemplate("one_dive.html");
	} else if (printOptions->p_template == print_options::TWO_DIVE) {
		grantlee_template = TemplateLayout::readTemplate("two_dives.html");
	} else if (printOptions->p_template == print_options::CUSTOM) {
		grantlee_template = TemplateLayout::readTemplate("custom.html");
	}

	ui->plainTextEdit->setPlainText(grantlee_template);

	int width = ui->label->width();
	int height = ui->label->height();
	QPixmap map(width * 2, height * 2);
	map.fill(QColor::fromRgb(255, 255, 255));
	Printer printer(&map, printOptions, templateOptions, Printer::PREVIEW);
	printer.previewOnePage();
	ui->label->setPixmap(map.scaled(width, height, Qt::IgnoreAspectRatio));
}

TemplateEdit::~TemplateEdit()
{
	delete ui;
}

void TemplateEdit::on_fontsize_valueChanged(int font_size)
{
	newTemplateOptions.font_size = font_size;
}

void TemplateEdit::on_linespacing_valueChanged(double line_spacing)
{
	newTemplateOptions.line_spacing = line_spacing;
}

void TemplateEdit::on_fontSelection_currentIndexChanged(int index)
{
	newTemplateOptions.font_index = index;
}

void TemplateEdit::on_colorpalette_currentIndexChanged(int index)
{
	newTemplateOptions.color_palette_index = index;
}

void TemplateEdit::on_TemplateEdit_finished(int result)
{
	if ((*templateOptions) != newTemplateOptions || grantlee_template.compare(ui->plainTextEdit->toPlainText())) {
		QMessageBox msgBox;
		msgBox.setText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msgBox.setDefaultButton(QMessageBox::Discard);
		if (msgBox.exec() == QMessageBox::Save) {
			memcpy(templateOptions, &newTemplateOptions, sizeof(struct template_options));
			printOptions->p_template = print_options::CUSTOM;
			TemplateLayout::writeTemplate("custom.html", ui->plainTextEdit->toPlainText());
		}
	}
}
