/**
 *
 * This file is part of the libvcard project.
 *
 * Copyright (C) 2010, Emanuele Bertoldi (Card Tech srl).
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Revision$
 * $Date$
 */

#include "vcard.h"
#include <QtCore/QFile>

vCard::vCard()
{
}

vCard::vCard(const vCard& vcard)
{
   m_properties = vcard.properties();
}

vCard::vCard(const vCardPropertyList& properties)
    :   m_properties(properties)
{
}

vCard::~vCard()
{
}

void vCard::addProperty(const vCardProperty& property)
{
    for (int i = 0; i < m_properties.count(); i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() == property.name() && current.params() == property.params())
        {
            m_properties[i] = property;
            return;
        }
    }

    m_properties.append(property);

}

void vCard::addProperties(const vCardPropertyList& properties)
{
    foreach (vCardProperty property, properties)
        this->addProperty(property);
}

vCardPropertyList vCard::properties() const
{
    return m_properties;
}

vCardProperty vCard::property(const QString& name, const vCardParamList& params, bool strict) const
{
    for (int i = 0; i < m_properties.count(); i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() == name)
        {
            vCardParamList current_params = current.params();

            if (strict)
            {
                if (params != current_params)
                    continue;
            }

            else
            {
                foreach (vCardParam param, params)
                    if (!current_params.contains(param))
                        continue;
            }

            return current;
        }
    }

    return vCardProperty();
}

bool vCard::contains(const QString& name, const vCardParamList& params, bool strict) const
{
    for (int i = 0; i < m_properties.count(); i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() != name)
            continue;

        vCardParamList current_params = current.params();

        if (strict)
        {
            if (params != current_params)
                continue;
        }

        else
        {
            foreach (vCardParam param, params)
                if (!current_params.contains(param))
                    continue;
        }

        return true;
    }

    return false;
}

bool vCard::contains(const vCardProperty& property) const
{
    return m_properties.contains(property);
}

bool vCard::isValid() const
{
    if (m_properties.isEmpty())
        return false;

    foreach (vCardProperty prop, m_properties)
        if (!prop.isValid())
            return false;

    return true;
}

int vCard::count() const
{
    return m_properties.count();
}

QByteArray vCard::toByteArray(vCardVersion version) const
{
    QStringList lines;

    lines.append(VC_BEGIN_TOKEN);

    switch (version)
    {
        case VC_VER_2_1:
            lines.append(vCardProperty(VC_VERSION, "2.1").toByteArray());
            break;

        case VC_VER_3_0:
            lines.append(vCardProperty(VC_VERSION, "3.0").toByteArray());
            break;

        default:
            return QByteArray();
    }

    foreach (vCardProperty property, this->properties())
        lines.append(property.toByteArray(version));
   
    lines.append(VC_END_TOKEN);

    return lines.join(QString(VC_END_LINE_TOKEN)).toUtf8();
}

bool vCard::saveToFile(const QString& filename) const
{
    QFile output(filename);
    if (output.open(QFile::WriteOnly))
    {
        output.write(this->toByteArray());
        output.close();

        return true;
    }
   
    return false;
}

QList<vCard> vCard::fromByteArray(const QByteArray& data)
{
    QList<vCard> vcards;
    vCard current;
    bool started = false;
   
    QList<QByteArray> lines = data.split(VC_END_LINE_TOKEN);
    foreach (QByteArray line, lines)
    {
        line = line.simplified();
        if ((line == VC_BEGIN_TOKEN) && !started)
        {
            started = true;
        }
        else if ((line == VC_END_TOKEN) && started)
        {
            vcards.append(current);
            started = false;
        }

        else if (started)
        {
            vCardPropertyList props = vCardProperty::fromByteArray(line);
            current.addProperties(props);
        }
    }

    if(vcards.empty())
        vcards << current;
   return vcards;
}

QList<vCard> vCard::fromFile(const QString& filename)
{
    QList<vCard> vcards;

    QFile input(filename);
    if (input.open(QFile::ReadOnly | QFile::Text))
    {
        vcards = vCard::fromByteArray(input.readAll());
        input.close();
    }

    return vcards;
}

bool vCard::operator ==(const vCard &param) const
{
    if(param.properties().count() != m_properties.count()) {
        return false;
    }
    for (int i = 0; i < m_properties.count(); i++)
    {
        vCardProperty current = m_properties.at(i);
        if (current.name() == param.properties().at(i).name() && current.value() != param.properties().at(i).value())
            return false;
    }
    return true;
}

QString vCard::getFullName()
{
    QString retour = property(VC_FORMATTED_NAME).toByteArray().replace(VC_FORMATTED_NAME+QString(":"),"").trimmed();
    if(retour == ":")
        return "";
    else
        return retour;
}

QDebug operator<<(QDebug dbg, const vCard &param)
{
    dbg.nospace() << param.toByteArray().replace("\n","");
    return dbg.maybeSpace();
}
