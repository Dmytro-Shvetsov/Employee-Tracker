import {FormFeedback, FormGroup, Input, Label} from "reactstrap";
import React from "react";


class TextInput extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            name: props.name,
            labelText: props.labelText,
            type: props.type,
        }
    }

    render() {
        const { name, labelText, type="text"} = this.state;
        return (
            <FormGroup>
                <Label for={name}>{labelText}</Label>
                <Input invalid={this.props.error !== undefined}
                       autoComplete={type === 'password' ? "new-password" : ''}
                       onChange={this.props.onInputChange}
                       type={type} name={name}
                       id={`#${name}`}
                       placeholder="*"/>
                <FormFeedback>{this.props.error}</FormFeedback>
            </FormGroup>
        );
    }
}

export default TextInput;