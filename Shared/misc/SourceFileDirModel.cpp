#include "SourceFileDirModel.h"

SourceFileDirModel::SourceFileDirModel(QObject *parent)
    : QDirModel(parent)
{

}

int SourceFileDirModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

